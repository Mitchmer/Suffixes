#include "Search.h"
#include <iostream>
using namespace std;

vector<size_t> searchFor(const string& pattern,
                         const string& text,
                         const SuffixArray& suffixArr) {

	size_t n = suffixArr.size();

	vector<size_t> matches = {};
/*
	cout << "pattern: " << pattern << endl;
	cout << "text: " << text << endl;

	cout << "---------------------------" << endl
		 << "Suffix Array" << endl
		 << "---------------------------" << endl;
	for (size_t i {}; i < n; i++) {
		cout << "suffixArr[" << i << "]: " << suffixArr[i] << endl;
		cout << text.substr(suffixArr[i]) << endl;
	}
	cout << "---------------------------" << endl << endl;
*/
	//==========================================================================================================================
	// Building the LCP Array
	//--------------------------------------------------------------------------------------------------------------------------
	// Reference:
	// GeeksforGeeks. “Kasai S Algorithm for Construction of LCP Array From Suffix Array.”
	//		GeeksforGeeks, Nov. 2021, www.geeksforgeeks.org/kasais-algorithm-for-construction-of-lcp-array-from-suffix-array/.
	//===========================================================================================================================

	// Step 1: Build LCP Array
	// geeksforgeeks kasai's algortihm
	vector<size_t> lcpArray(n, 0);

	// auxiliary array to store inverse of suffix array
	// in order to find the next suffix string from the suffix array
	// suffArr[0] = 5; invSuff[5] = 0;
	vector<size_t> invSuffix(n, 0);

	for (size_t i {}; i < n; i++)
		invSuffix[suffixArr[i]] = i;

	// Previous LCP value
	size_t k {};

	// assign LCP for each pair of suffixzes starting from the first suffix within the text string

	for (size_t i {}; i < n; i++) {

		// last element in LCP is comparing last element of suffArr with a nil value = always 0.
		if (invSuffix[i] == n-1) {
			k = 0;
			continue;
		}

		size_t j = suffixArr[(invSuffix[i]+1)];
		// comparing two suffix strings character by character until a mismatch or end of one of the suffixes
		while ((i+k) < n && (j+k) < n && text[i+k]==text[j+k])
			k++;

		// add the value to the LCP array
		lcpArray[invSuffix[i]] = k;

		// the next string will always be 1 less character than the previous one.
		if (k>0)
			k--;

	}
/*
	cout << "---------------------------" << endl
		 << "LCP Array" << endl
		 << "---------------------------" << endl;
	for (size_t i {}; i < n; i++)
		cout << "lcpArray[" << i << "]: " << lcpArray[i] << endl;
	cout << "---------------------------" << endl << endl;
*/
	// recursive binary search - p.1271 textbook 3rd ed.

	// find the midpoint and compare with pattern using str.compare() ((first + last)/2)
			// if the pattern is greater than the substring at the midpoint (from str.compare()), result is in the second half -
				// if the string length of the midpoint is greater, check the prefix with a length of the pattern
			//if less, in first half
			// recursively cut in half until a match is found

	int first {};
	int last {static_cast<int>(n) - 1};
	int midpoint {};
	int compare {};
	bool found {false};

	while (first <= last && !found) {

//		cout << "First: " << first << endl;
//		cout << "Last: " << last << endl;

		midpoint = (first+last)/2;
//		cout << "Midpoint: " << midpoint << endl;

		compare = pattern.compare(text.substr(suffixArr[midpoint]));
//		cout << "Compared Value: " << compare << endl;

		if (compare == 0) { // substring matches pattern
//			cout << "Found Match" << endl;
			found = true;
		} else if (compare > 0) { 
			// pattern is greater than the stubstring
			// if a match exists, it must be in the greater indexes
//			cout << "Pattern is greater" << endl;
			first = midpoint + 1;
		} else {
			// pattern is less than the stubstring
			// if a match exists, there are two results:
				// 1. it must be a prefix of the substring
				//	if not,
				// 2. it must be in the lesser indexes
//			cout << "Pattern is less than substring at midpoint" << endl;
			// check is prefix matches the pattern
			if (text.substr(suffixArr[midpoint], pattern.length()) == pattern) {

//				cout << "Midpoint substring: "<< text.substr(suffixArr[midpoint]);
//				cout << "Pattern is a prefix" << endl;
				found = true;
				//matches.push_back(suffixArr[midpoint]);
			} else {
				last = midpoint - 1;
			}
		}
	//	cin.get();
	}
//	cout << "Match Index: " << midpoint << endl;
	//==================================
	// Utilize the LCP
	//==================================

	// If a match is found

	int matchIndex {midpoint};

	// search backwards
	bool leftFound {false};
	bool rightFound {false};
	bool duplicates {false};

	// while the index is greater than the first index (0) a left (or upper) boundary has not been found
//	cout << endl << "Starting left bound check.... " << endl;
	while (found && matchIndex > 0 && !leftFound) {
		// check if the LCP value is greater than or equal to the pattern's length, indicating a duplicate
//		cout << "lcpArray[" << matchIndex << "]: " << lcpArray[matchIndex] << endl;
		if (lcpArray[matchIndex-1] >= pattern.length()) {
			matchIndex--;
			//matches.push_back(suffixArr[matchIndex]);
		} else {
			// set the "left boundary" to be the index right before the mismatch
			//matchIndex++;
			leftFound = true;
		}
	}

	//matchIndex = midpoint;
//	cout << endl << "Starting right boudn check..." << endl;
	int num_elements = static_cast<int>(n);
	// now search forward, and add the matching indexes to the matches vector
	while (found && matchIndex < num_elements && !rightFound) {

//		cout << "lcpArray[" << matchIndex << "]: " << lcpArray[matchIndex] << endl;

		if (lcpArray[matchIndex] >= pattern.length()) {
			matches.push_back(suffixArr[matchIndex]);
			matchIndex++;
			duplicates = true;
		} else if (duplicates) {
			matches.push_back(suffixArr[matchIndex]);
			rightFound = true;
		} else
			rightFound = true;
	}

	// if a match was found but there were no duplicates
	if (found && duplicates == 0)
		matches.push_back(suffixArr[midpoint]);

		// check the LCP value before the current index
	// find a match
	// then use the LCP to move up the suffix array until the value of the LCP at that index is less
		// than the length of the pattern,
		// then move down
			// this will find duplicates

  return matches;

}
