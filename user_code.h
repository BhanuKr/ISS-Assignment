#ifndef USER_CODE_H
#define USER_CODE_H

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <algorithm>
#include "fileIterator.h"
#include "fileWriter.h"
#include <map>
#include <set>

using namespace std;


//////////////////////////////////////////////////////////////////////////////////
// MODIFY THIS SECTION
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Modify this code to solve the assigment. Include relevant document. Mention the prompts used prefixed by #PROMPT#.
 *        
 * 
 * @param hashtags 
 * @param purchases 
 * @param prices 
 */

//O(1) time complexity and O(1) space complexity
//Function to split a string by a delimiter
vector<string> split(const string &s, char delimiter) 
{ 
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


// O(N**2) time complexity and O(N) space complexity
// Function to group customers by hashtags
vector<vector<int>> Groups(vector<pair<int, int>> interactions, map<int, vector<string>> products,int k)
{
    // Map to store each customer's hashtag frequency
    map<int, map<string, int>> customer_hashtags;

    //O(N) both time and space complexity
    // Step 1: Populate the map with hashtags frequency
    for (auto& interaction : interactions) {
        int customer_id = interaction.first;
        int product_id = interaction.second;

        // Get the hashtags for the product
        vector<string> hashtags = products[product_id];

        //O(1) Since the number of hashtags for a product is very small compared to the number of products we will have for asymptotic analysis .
        // Update the customer's hashtag frequency
        for (const string& hashtag : hashtags) {
            customer_hashtags[customer_id][hashtag]++;
        }
    }

    // Step 2: Take the first k hashtags from the sorted list for each customer
    map<int, set<string>> customer_top_k_hashtags;
    for (const auto& customer : customer_hashtags) {
        int customer_id = customer.first;
        const map<string, int>& hashtags_map = customer.second;

        // Convert the map to a vector of pairs for sorting by frequency
        vector<pair<string, int>> hashtag_freq(hashtags_map.begin(), hashtags_map.end());

        //O(NlogN) time complexity .
        // Sort the vector by frequency in descending order, then lexicographically
        sort(hashtag_freq.begin(), hashtag_freq.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            if (a.second != b.second) return a.second > b.second;
            return a.first < b.first;
        });

        // Take the first k hashtags for this customer
        set<string> top_k_hashtags;
        for (int i = 0; i < k && i < hashtag_freq.size(); i++) {
            top_k_hashtags.insert(hashtag_freq[i].first);
        }

        // Store the top k hashtags for this customer
        customer_top_k_hashtags[customer_id] = top_k_hashtags;
    }

    // Step 3: Find groups of similar customers
    vector<vector<int>> groups;
    set<int> added_customers;

    //O(N^2) , O(N) space complexity
    for (const auto& customer : customer_top_k_hashtags) {
        int customer_id = customer.first;
        
        // If the customer is already added to a group, skip
        if (added_customers.find(customer_id) != added_customers.end()) {
            continue;
        }

        // Start a new group with this customer
        vector<int> group;
        group.push_back(customer_id);
        added_customers.insert(customer_id);

        // Find similar customers and add them to the same group
        for (const auto& other_customer : customer_top_k_hashtags) {
            int other_customer_id = other_customer.first;
            if (customer_id != other_customer_id && customer_top_k_hashtags[customer_id] == customer_top_k_hashtags[other_customer_id]) {
                group.push_back(other_customer_id);
                added_customers.insert(other_customer_id);
            }
        }

        // Add the group to the list of groups
        groups.push_back(group);
    }
    return groups;
}

// Initalised products globally for the function groupCustomersByHashtagsForDynamicInserts().
// So that the update of new batch happens on top of the previous batch.
map<int, vector<string>> products;
vector<pair<int, int>> interactions; // Since interactions is not changing for q3 , we can reuse it from q1 .
int counter = 0;
// counter is used for using the initial products map and then updating it with the new hashtags.

// O(M + N**2) time complexity and O(N+M) space complexity
void groupCustomersByHashtags(fileIterator& hashtags, fileIterator& purchases,fileIterator& prices, int k, string outputFilePath)
{


    //Use this to log compute time    
    auto start = high_resolution_clock::now();


// O(M) both time and space complexity .    
    while (hashtags.hasNext()) {
        string line = hashtags.next();
        vector<string> tokens = split(line, ',');
        if (tokens.size() < 2) {
            continue;
        }
        int product_id = stoi(tokens[0]);
        //O(1)
        for (int i = 1; i < tokens.size(); i++) {
            products[product_id].push_back(tokens[i]);
        }
    }

    // Vector to store the customer_id and product_id interactions
    while (purchases.hasNext()) {
        string line = purchases.next();
        vector<string> tokens = split(line, ',');
        if (tokens.size() < 2) {
            continue;
        }
        int customer_id = stoi(tokens[0]);
        int product_id = stoi(tokens[1]);
        interactions.push_back(make_pair(customer_id, product_id));
    }

    
    // Call the function to group customers by hashtags
    vector<vector<int>> groups = Groups(interactions, products, k);


    //O(N) time complexity and O(1) space complexity
    // Write groups to output file
    for (const auto& group : groups) {
        writeOutputToFile(group, outputFilePath); 
    }


    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by compute part of the function: " << duration.count() << " microseconds" << endl;

    return;
}

//////////////////////////////////////////////////////////////////////////////////
// MODIFY THIS SECTION
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Modify this code to solve the assigment. Include relevant document. Mention the prompts used prefixed by #PROMPT#.
 *        
 * 
 * @param customerList 
 * @param purchases 
 * @param prices
 */
//O(N+M) time complexity , O(M) space complexity
float calculateGroupAverageExpense(vector<int> customerList, fileIterator& purchases,fileIterator& prices){

    //Use this to log compute time    
    //O(M) both time and space complexity
    auto start = high_resolution_clock::now();
    map<int, float> product_prices;
    while(prices.hasNext()){
        string line = prices.next();
        vector<string> tokens = split(line, ',');
        if (tokens.size() < 2){
            continue;
        }
        int product_id = stoi(tokens[0]);
        float price = stof(tokens[1]);
        product_prices[product_id] = price;
    }
    
    //O(N) time complexity . O(1) space complexity .
    float total_expense = 0.0;
    for (int customer_id : customerList) {
        purchases.reset();
        while(purchases.hasNext()){
            string line = purchases.next();
            vector<string> tokens = split(line, ',');
            if (tokens.size() < 2){
                continue;
            }
            int customer_id_ = stoi(tokens[0]);
            int product_id = stoi(tokens[1]);
            if (customer_id == customer_id_){
                total_expense += product_prices[product_id];
            }
        }
    }

    if (customerList.size()!= 0) {
    float average_expense = total_expense / customerList.size();
    cout << "Average expense of the group: " << average_expense << endl;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by this function: "<< duration.count() << " microseconds" << endl;

    return average_expense;
    }
    return -1; // To show the end of file . //outputFile << '\n'; in fileWriter.h rens the loop of empty line once .
}


//////////////////////////////////////////////////////////////////////////////////
// MODIFY THIS SECTION
//////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Modify this code to solve the assigment. Include relevant document. Mention the prompts used prefixed by #PROMPT#.
 *        
 * 
 * @param hashtags 
 * @param purchases 
 * @param prices
 * @param newHashtags
 * @param k
 * @param outputFilePath
 */
//O()
void groupCustomersByHashtagsForDynamicInserts(fileIterator& hashtags, fileIterator& purchases,fileIterator& prices,vector<string> newHashtags, int k, string outputFilePath){
    //Use this to log compute time    
    auto start = high_resolution_clock::now();
    // the strings in newHashtags are in the format "product_id,hashtags" eg "1,#hashtag1,#hashtag2"
    // from hashtags get the product_id and the hashtags old . Then update the product_id with the new hashtags i.e add the hastags to the product_id.
    // the strings in newHashtags are like 4,tech,accessory,electronics.The code given is not considering more than one hashtag for a product_id.
    
    // On assuming that the number of products for update will be very less compared to the number of products we have in the initial hashtags file.
    //O(M) time complexity and O(M) space complexity 
    if (counter==0){
    while (hashtags.hasNext()) {
        string line = hashtags.next();
        vector<string> tokens = split(line, ',');
        if (tokens.size() < 2) {
            continue;
        }
        int product_id = stoi(tokens[0]);
        for (int i = 1; i < tokens.size(); i++) {
            products[product_id].push_back(tokens[i]);
        }
    }
    counter++;
    }

    //O(1) time complexity and space complexity ,On assuming that the number of products  
    //for update will be very less compared to the number of products we have in the initial hashtags file.
    // Update the products with the new hashtags
    for (const string& newHashtag : newHashtags) {
        vector<string> tokens = split(newHashtag, ',');
        if (tokens.size() < 2) {
            continue;
        }
        int product_id = stoi(tokens[0]);
        for (int i = 1; i < tokens.size(); i++) {

        products[product_id].push_back(tokens[i]);
        }
    }

    // O(N**2) time complexity and O(N) space complexity
    // Call the function to group customers by hashtags
    vector<vector<int>> groups = Groups(interactions, products, k);

    // Write groups to output file O(N) time complexity and O(1) space complexity
    for (const auto& group : groups) {
        writeOutputToFile(group, outputFilePath);
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by compute part of the function: "<< duration.count() << " microseconds" << endl;

    return;
    
}



#endif // USER_CODE_H
