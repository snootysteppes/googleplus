import requests
from bs4 import BeautifulSoup
import json
import re
import time

# Function to perform a Google search and extract clean URLs
def google_search(query, num_results=30):
    url = f"https://www.google.com/search?q={query}&num={num_results}"
    headers = {'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3'}
    try:
        response = requests.get(url, headers=headers)
        soup = BeautifulSoup(response.text, 'html.parser')
        results = soup.find_all('div', class_='kCrYT')
        urls = []
        for res in results:
            anchor_tag = res.find('a')
            if anchor_tag:
                link = anchor_tag.get('href')
                # Extract clean URL using regular expression
                clean_url = re.search(r'(https?://[^&]+)', link)
                if clean_url:
                    urls.append(clean_url.group(1))
        return urls
    except Exception as e:
        print(f"Error performing Google search for '{query}': {e}")
        return []

# Categories to search for
categories = [
    "shopping websites",
    "news websites",
    "social media websites",
    "entertainment websites",
    "education websites",
    "food websites",
    "sports websites",
    "music websites",
    "travel websites",
    "health websites",
    "technology websites",
    "finance websites",
    "gaming websites",
    "fashion websites",
    "art websites",
    "business websites",
    "career websites",
    "DIY websites",
    "parenting websites",
    "science websites",
    "environment websites",
    "cooking websites",
    "fitness websites",
    "history websites",
    "language learning websites",
    "literature websites",
    "photography websites",
    "religious websites",
    "shopping deal websites"
]

# Dictionary to store category-wise URLs
category_urls = {}  
# Perform Google search for each category and collect URLs
for category in categories:
    print(f"Searching for {category}...")
    urls = google_search(category)
    if urls:
        category_urls[category] = urls
        print(f"Found {len(urls)} URLs for {category}.")
    time.sleep(2)  # Pause for 2 seconds between each search to avoid overloading Google (and not get blocked by them for spam)

# Write category-wise URLs to JSON file
with open('websites.json', 'w') as f:
    json.dump(category_urls, f, indent=4)

print("Category-wise URLs saved to websites.json")
