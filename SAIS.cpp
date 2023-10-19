#include "SAIS.h"
#include <iostream>
using namespace std;

SuffixArray sais(const vector<size_t>& text) {
	const int ALPHABET_SIZE {26};
	size_t n {text.size()};
	// Prints the incoming array
	cout << "Text Array: ";
	for (size_t i {}; i < n; i++)
		cout << text[i] << " ";
	cout << endl;


	// Create an array of chars to hold the L's and S's
	vector<char> charArr(n, 0);
	vector<size_t> lmsArr(1, n-1);
	vector<int> suffArr (n, -1);

	//=================================================================
	// Bucket Array
	//-----------------------------------------------------------------

	// Keeps track of the size of each bucket.
	//
	// The character's place within the array is determined by its rank
	//		(The rank 1's go in idex 1, rank 2's go in index 2, etc.)
	//			The sentinel goes in index 0.

	vector<int> bucketArray (ALPHABET_SIZE + 1, 0);

	// Set the last element to an LMS (last element is a sentinel value),
	//		the first bucket in the bucket array corresponds to the bucket of the sentinel value (0),
	//		which will always only consist of one element.
	bucketArray.at(0) = 1;

	//=================================================================
	// Building the character Array, LMS array, and bucket array
	//-----------------------------------------------------------------

	// Start by setting the last character, the sentinel value, as a smaller value.
	charArr[n-1] = 'S';

	// GO backwards through the string, starting at the second to last element, and check the element directly AFTER it.
	// Larger characters are represented by an L
	// LMS character are represented by an S
	// non-LMS smaller characters are represented by an s

	// We are using a method that takes into account size_t's issue with decrements: size_t can never be negative, and so >= 0 loops are not as straightforward
	// 		therefore, instead of checking the array[i] against array[i+1], we're checking array[i-1] with array[i]
	// 		this will allow us to accesss the index at 0 wihtout needing our loop variable to actually be 0 at the end.

	// At the same time, we can also determine the sizes of each bucket.
	//		for each element, increase the size of that element's bucket size within the bucketArray.

	for (size_t i {n-1}; i > 0; i--) {
		bucketArray[text[i-1]]++;
		// if the value in the text array at i-1 is greater than the one after it, then that value is a larger one = 'L'
		if (text[i-1] > text[i]) {
			charArr[i-1] = 'L';
			// if this value is an L, then the value directly after it would be an LMS if it is smaller. 
			if (charArr[i] == 's'){
				charArr[i] = 'S';
				// add this index at the beginning of the LMS array
				lmsArr.insert(lmsArr.begin(),i);
			}
		// if the current value is the same as the one after it, then the character will always be the same as that one.
		} else if (text[i-1] == text[i]) {
			if (charArr[i] == 'S'|| charArr[i] == 's')
				charArr[i-1] = 's';
			else
				charArr[i-1] = 'L';
		} else
			charArr[i-1] = 's';
	}

	// Loop through the bucklet array, keeping track of the number of items before it (this will represent the number of previous indexes)
	int bucketIndexes {};
	for (size_t i {1}; i < bucketArray.size(); i++) {
		bucketIndexes += bucketArray.at(i);
		bucketArray.at(i) = bucketIndexes;
	}
	bucketArray.at(0) = 0;

	cout << "Char Array: ";
	for (size_t i {}; i < n; i++)
		cout << charArr[i] << " ";
	cout << endl;

	cout << "LMS Indexes: ";
	for (size_t i {}; i < lmsArr.size(); i++)
		cout << lmsArr[i] << " ";
	cout << endl;

	cout << "Bucket Array: ";
	for (size_t i {}; i < bucketArray.size(); i++)
		cout << "|" << i << ": " << bucketArray[i] << "|";
	cout << endl;


	//=================================================================
	// Sorting the LMS's
	//-----------------------------------------------------------------

	// Working backwards through the LMS array, for each element, place that element at the end of its respective bucket.

//	cout << "Backwards LMS indexes: ";
	// Since the sentinel value is always an LMS, and it is at the end, we can skip it and start with the second to last element.
	suffArr.at(0) = n-1;
	{
		vector<int> sBucketArr = bucketArray;
		vector<int> lBucketArr = bucketArray;
		cout << endl;
		cout << "===============================" << endl;
		cout << "Inserting LMS 'S' values" << endl;
		cout << "===============================" << endl << endl;

		for (size_t i {2}; i <= lmsArr.size(); i++) {
			cout << "LMS: " << lmsArr.at(lmsArr.size()-i) << endl;
			cout << "Value at LMS " << lmsArr.at(lmsArr.size()-i) << ": " << text.at(lmsArr.at(lmsArr.size()-i)) << endl;
			cout << "Size of bucket " << text.at(lmsArr.at(lmsArr.size()-i)) << ": " << bucketArray.at(text.at(lmsArr.at(lmsArr.size()-i))) << endl;
			cout << "Last index of bucket " << text.at(lmsArr.at(lmsArr.size()-i)) << ": " << bucketArray.at(text.at(lmsArr.at(lmsArr.size()-i))) << endl << endl;

			int suffIndex = sBucketArr.at(text.at(lmsArr.at(lmsArr.size()-i)));

			// while the suffix at the index is already taken, decrement until one is empty (a value of -1)

			// Here, instead of performing a while loop, we could also simply clone the bucketArray and decrement those values instead.
			suffArr.at(suffIndex) = lmsArr.at(lmsArr.size()-i);
			// Decrement the size of the bucket to accomodate for the index being taken
			sBucketArr.at(text.at(lmsArr.at(lmsArr.size()-i)))--;
		}

		// move forward through the suffix array (which is now filled with LMS's) and put the L indexes in their correct buckets
		// temporary suffix vector holding only the LMS's
		//vector<int> lmsSuffArr = suffArr;
		cout << endl;
		cout << "===============================" << endl;
		cout << "Inserting 'L' values" << endl;
		cout << "===============================" << endl << endl;
		for (size_t i {}; i < suffArr.size(); i++) {
			if (suffArr.at(i) != -1) { // if the space is not empty
				int currentLIndex {suffArr.at(i)-1};
			// while the character in the charArray at this index is an 'L', update the suffix array
			// at the rightmost available index for that value's (rank's) bucket
			// If the index is 0, don't try to check the index before it - just skip.
				if ((charArr.at(currentLIndex) == 'L') && (currentLIndex != 0)){
					cout << "Index: " << suffArr.at(i) << endl;
					cout << "Previous index: " << currentLIndex << endl;
					cout << "Character at the previous index: " << charArr.at(currentLIndex) << endl;
					cout << "Value of previous index: " << text[currentLIndex] << endl;
					cout << "First open slot of the previous index's bucket: " << lBucketArr.at(text[currentLIndex]-1)+1 << endl << endl;
					// Once an L value is found, we increment the index of the previous bucket, 
					// This would then invade the beginning of the correct bucket at first, and keep track of indexes at the beginning of the bucket that have not been taken.
					lBucketArr.at(text[currentLIndex]-1)++;
					suffArr.at(lBucketArr.at(text[currentLIndex]-1)) = static_cast<int>(currentLIndex);
					// We need to continue to check indexes directly before each 'L' value until we hit an 'S' or 's'
			//		if (currentLIndex >= 0)
			//			currentLIndex--;
			//		else
			//			break;
				}
			}
		}
		/*
		// FInd the indexes of L-types, and place them at the beginning of their buckets
		for (size_t i {}; i < charArr.size(); i++) {
			if (charArr.at(i) == 'L') {
				// if it's an L
				cout << "Index: " << i << endl;
				cout << "Previous bucket: " << lBucketArr.at(text[i]-1) << endl << endl;
				// Once an L value is found, we increment the index of the previous bucket, 
				// This would then invade the beginning of the correct bucket at first, and keep track of indexes at the beginning of the bucket that have not been taken.
				lBucketArr.at(text[i]-1)++;
				suffArr.at(lBucketArr.at(text[i]-1)) = static_cast<int>(i);
			}
			//int suffIndex = lBucketArr.at(text.at(lmsArr.at(lmsArr.size()-i)));


		}
		*/
	}
	cout << endl;

	cout << "Suffix Array: ";
	for (size_t i {}; i < suffArr.size(); i++){
		cout << suffArr.at(i) << " ";
	}
	cout << endl;
		// keep track of the current position of the L's (or S's) within the new sorted suffix array when popping indexes into it
			// each new suffix comes before all S's
				// and before all L's
			// for each element i, if it's an L, pop it into suffArr[0] and increase j
				// if it's an S, pop it into suffArr[j]

  // place the suffixes into their respective buckets

  return {};
}
