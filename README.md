# Google Plus
## What is it?
A faster version of Google, ~17,481x quicker.
## How to use?
Use the websites preset in `src/websites.json` or run `scraper/main.py` to make your own websites.json website database. Compile the C++ code, then put it in the folder where the source code is, so it can access the "src" directory with your website database, then run a search. It not only gives you your website results, but how fast it took, compared to Google.
## Why is it faster?
* Limited Dataset: The program operates on a relatively small dataset loaded from a local file. Google, on the other hand, searches through an index of the entire web, which is significantly larger and more complex.

* Local Execution: The program runs locally on a single machine, whereas Google Search operates on a distributed network of servers. Local execution may appear faster due to lower latency and fewer network overheads.

* Simplified Functionality: The program performs a basic search operation without the complexities of web crawling, indexing, and ranking used by Google. Its simplicity may result in faster execution for specific tasks.

* Different Use Cases: The program may excel in specific use cases where it's optimized, such as searching within a predefined dataset. Google Search, on the other hand, is designed to handle a wide range of search queries across various domains, which can affect its response time.
* TL;DR: Google needs to process lots more data further away causing latency issues on a larger dataset than just a few hundred websites.
## Graphs
[![image.png](https://i.postimg.cc/RVVPTkz6/image.png)](https://postimg.cc/MXNb6rHq)
[![image.png](https://i.postimg.cc/hv7VC1MY/image.png)](https://postimg.cc/D4nW0s9Q)
