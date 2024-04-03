#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <sstream>
#include <thread>
#include <algorithm>
#include <condition_variable>
#include <future>
#include <deque>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class ThreadPool {
public:
    explicit ThreadPool(size_t num_threads) : stop(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) {
                            return;
                        }
                        task = move(tasks.front());
                        tasks.pop_front();
                    }
                    task();
                }
            });
        }
    }

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> future<typename result_of<F(Args...)>::type> {
        using return_type = typename result_of<F(Args...)>::type;
        auto task = make_shared<packaged_task<return_type()>>(bind(forward<F>(f), forward<Args>(args)...));
        future<return_type> res = task->get_future();
        {
            lock_guard<mutex> lock(queue_mutex);
            if (stop) {
                throw runtime_error("enqueue on stopped ThreadPool");
            }
            tasks.emplace_back([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    ~ThreadPool() {
        {
            lock_guard<mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (thread& worker : workers) {
            worker.join();
        }
    }

private:
    vector<thread> workers;
    deque<function<void()>> tasks;
    mutex queue_mutex;
    condition_variable condition;
    bool stop;
};

unordered_map<string, vector<string>> load_websites(const string& filename) {
    string file_path = "src/" + filename;
    ifstream file(file_path);
    if (!file.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        exit(EXIT_FAILURE);
    }

    json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        cerr << "Error parsing JSON: " << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    unordered_map<string, vector<string>> websites;
    for (const auto& entry : j.items()) {
        string tag = entry.key();
        vector<string> urls = entry.value().get<vector<string>>();
        websites[tag] = move(urls);
    }
    return websites;
}

void search_tags(
    const vector<string>& tags,
    const unordered_map<string, vector<string>>& reverse_index,
    double& total_time,
    unordered_map<string, vector<string>>& results
) {
    auto start_time = chrono::steady_clock::now();
    for (const auto& tag : tags) {
        // Trim whitespace from the tag
        string trimmed_tag = tag;
        trimmed_tag.erase(0, trimmed_tag.find_first_not_of(" \t\r\n"));
        trimmed_tag.erase(trimmed_tag.find_last_not_of(" \t\r\n") + 1);

        // Case-insensitive search for the tag
        auto it = reverse_index.find(trimmed_tag);
        if (it != reverse_index.end()) {
            results[tag] = it->second;
        } else {
            results[tag] = {}; // Empty vector if tag not found
        }
    }
    auto end_time = chrono::steady_clock::now();
    double elapsed_time = chrono::duration<double, milli>(end_time - start_time).count();

    // Update total time under mutex lock
    {
        static mutex total_time_mutex;
        lock_guard<mutex> lock(total_time_mutex);
        total_time += elapsed_time;
    }
}

int main() {
    string filename = "websites.json";
    unordered_map<string, vector<string>> reverse_index = load_websites(filename);

    string input;
    cout << "Enter tags to search for (comma-separated): ";
    getline(cin, input);
    stringstream ss(input);
    vector<string> tags;
    string tag;
    while (getline(ss, tag, ',')) {
        tags.push_back(tag);
    }

    double total_time = 0.0;
    unordered_map<string, vector<string>> results;
    ThreadPool pool(thread::hardware_concurrency());

    vector<future<void>> futures;
    futures.emplace_back(pool.enqueue(search_tags, ref(tags), cref(reverse_index), ref(total_time), ref(results)));
    for (auto& future : futures) {
        future.wait();
    }

    for (const auto& tag : tags) {
        const vector<string>& urls = results[tag];
        if (!urls.empty()) {
            cout << "Redirecting to:" << endl;
            for (const auto& url : urls) {
                cout << url << endl;
            }
        } else {
            cout << "No matching website found for the tag: " << tag << endl;
        }
    }
    cout << "Total time taken for all searches: " << total_time << " milliseconds" << endl;

    return 0;
}

