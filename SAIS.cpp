#include "SAIS.h"

#include <iostream>
#include <iomanip>
#include "DC3.h"

using namespace std;

//Define constants
const int ALPHABET_SIZE = 256;

//Declare structs
struct annotation {
	int index = -1; //[REDUNDANT - will be stored in index-order vector] The index of the current annotation
	int value = -1; //The value of the character (i.e. what character does it represent? a, b, c, etc...?)
	char type; //Is this character an l-type or an s-type? (L, s respectively)
	bool LMS = false; //Is this value a left-most s-type?
};

struct bucketInfo{
	int bucketName = -1; //The value of the character the bucket holds information for (i.e. what character does this bucket represent? a, b, c, etc...?)
	int startingIndex = -1; //The first (left-most) index that belongs to the bucket (i.e. inside it, being the left-most bound)
	int endingIndex = -1; //The last (right-most) index that belongs to the bucket (i.e. inside it, being the right-most bound)
	int emptySlotLeft = -1; //For induced sorting: The first slot at the front (left side) of the bucket that can be overwritten
	int emptySlotRight = -1; //For induced sorting: The first slot at the back (right side) of the bucket that can be overwritten
};

struct lmsSubstringBucket {
	int index = -1; //The first (left-most) index that belongs to the bucket (i.e. inside it, being the left-most bound)
	int value = -1; ////The value of the LMS character (i.e. what character does it represent? a, b, c, etc...?)
	int endingIndex = -1; //The last (right-most) index that belongs to the bucket (i.e. inside it, being the right-most bound)
	string bucketName = ""; //A substring of the input text taken from the starting index to the ending index
};

//Function prototypes
void createBuckets(vector<bucketInfo> &buckets, vector<int> &charCounts);
vector<int> inducedSort(vector<int> &suffArr, vector<lmsSubstringBucket> &lmsArray, vector<bucketInfo> &buckets, vector<annotation> &notes, vector<int> &charCounts, size_t &textLength);

SuffixArray sais(const vector<size_t>& text) {
	//Declare variables
	size_t textLength = text.size(); //Number of characters in the input text
	vector<int> suffArr (textLength, -1); //The operable suffix array: int to make use of -1 as a default initialization value
	SuffixArray finalSuffArr (textLength); //The final suffix array
	vector<annotation> notes(textLength); //Annotation information per character in input text
	vector<lmsSubstringBucket> lmsArray; //Stores the original location and relevant information for each LMS substring
	vector<bucketInfo> buckets (ALPHABET_SIZE); //Information for each bucket
	
	
	//Precomputation: Compute buckets
	//===================================================================================================
		vector<int> charCounts (ALPHABET_SIZE, 0); //Number of times each unique character appears in the input text
	//---------------------------------------------------------------------------------------------------
	//Count characters
	for (size_t i = 0; i < textLength; ++i) {
		++charCounts[text.at(i)];
	}
	
	//Create buckets
	createBuckets(buckets, charCounts);
	
	//---------------------------------------------------------------------------------------------------
	//===================================================================================================
	
	
	//Step 1: Annotation
	//===================================================================================================
	//---------------------------------------------------------------------------------------------------
	//Reverse pass
	// TODO: create currentSubstring string (or vector) to use
	string currentSubstring {}; // string to hold the lms block for each index to build upon, which will be appended to the next lms

	for (int i = static_cast<int>(textLength)-1; i >= 0 ; --i) {
		//Collect information
		notes.at(i).index = i;
		notes.at(i).value = text.at(i);

		//Check for sentinel case
		if (text[i] == 0) {
			notes.at(i).type = 's';
			notes.at(i).LMS = true;
		} else if (text[i] > text[i+1]) { //If the current char is greater than the one to the right
			notes.at(i).type = 'L';
		} else if (text[i] < text[i+1]) { //If the current char is less than the one to the right
			notes.at(i).type = 's';
		} else { //Both values are equal
			notes.at(i).type = notes.at(i+1).type;
		}

		//Check for LMS type
		if (notes.at(i).type == 'L' && notes.at(i+1).type == 's') {
			notes.at(i+1).LMS = true;
			//TODO:

			//Collect LMS information
			lmsSubstringBucket currentLMS;
			currentLMS.index = i+1;
			currentLMS.value = notes.at(i+1).value;
			currentLMS.bucketName = currentSubstring; // Add currentSubstring to its (i+1) member

			lmsArray.insert(lmsArray.begin(), currentLMS);

			currentSubstring = ""; 	// Clear currentSubstring
		}

		currentSubstring.insert(0, to_string(notes.at(i).value)); // add character value to the front of currentSubstring
	}

	//Finalize LMS array
	//---------------------------------------------------------------------------------------------------
	//Convert the input text vector to a string
	string textAsString;
	for (size_t i = 0; i < textLength; ++i) {
		textAsString += to_string(text.at(i));
	}

	//Compute strings and ending indices
	for (size_t i = 0; i < lmsArray.size(); ++i) {
		//If this is not the last LMS
		if (i+1 != lmsArray.size()) {
			//Set the ending index of the current LMS bucket to the starting index of the next one
			lmsArray.at(i).endingIndex = lmsArray.at(i+1).index;
		} else {
			//Set the ending index of the current LMS bucket to its own starting index
			lmsArray.at(i).endingIndex = lmsArray.at(i).index;
		}
		
		//Compute the number of characters long the current LMS substring is
		//TODO: int length = lmsArray.at(i).endingIndex - lmsArray.at(i).index + 1;
		
		//Set the substring name
		//TODO: lmsArray.at(i).bucketName = textAsString.substr(lmsArray.at(i).index, length);
	}
	
	//Optional Assertion
//	cout << "TEXT AS STRING: " << textAsString << endl << endl;
//	cout << endl << "Now printing LMS array information..." << endl;
//	for (size_t i = 0; i < lmsArray.size(); ++i) {
//		cout << "-----------------------------------------------" << endl;
//		cout << "Information for LMS bucket " << i << ": " << endl;
//		cout << "  Stored string-: " << lmsArray.at(i).bucketName << endl;
//		cout << "  Starting index: " << lmsArray.at(i).index << endl;
//		cout << "  Ending index--: " << lmsArray.at(i).endingIndex << endl;
//	}
//	cout << endl;
	//---------------------------------------------------------------------------------------------------
	
	//Optional Assertion
//	cout << endl << "Now Printing current data..." << endl;
//	for (size_t i = 0; i < textLength; ++i) {
//		cout << "------------------------" << endl;
//		cout << "Current Index: " << i << endl;
//		cout << "Value--------: " << notes.at(i).value << endl;
//		cout << "Type---------: " << notes.at(i).type << endl;
//		cout << "LMS?---------: " << boolalpha << notes.at(i).LMS << endl;
//	}
//	for (int i = 0; i < static_cast<int>(buckets.size()); ++i) {
//		cout << buckets[i].startingIndex << " ";
//	}
//	cout << endl;
//	for (int i = 0; i < static_cast<int>(buckets.size()); ++i) {
//		cout << buckets[i].bucketName << " ";
//	}
//	cout << endl;
//	for (int i = 0; i < static_cast<int>(buckets.size()); ++i) {
//		cout << buckets[i].emptySlotLeft << " ";
//	}
//	for (int i = 0; i < static_cast<int>(lmsArray.size()); ++i) {
//		cout << lmsArray[i].index << " ";
//	}
//	cout << endl << endl;
	//---------------------------------------------------------------------------------------------------
	//===================================================================================================
	
	
	//Step 2: Induced sorting
	//===================================================================================================
	//---------------------------------------------------------------------------------------------------
	suffArr = inducedSort(suffArr, lmsArray, buckets, notes, charCounts, textLength);
	//---------------------------------------------------------------------------------------------------
	//===================================================================================================
	
	
	//Step 3: Glean the reduced string
	//===================================================================================================
		vector<int> substringNames (textLength, -1); //The map that will hold the LMS substring names at their respective indices
		vector<lmsSubstringBucket> lmsArrayMap (textLength); //A map storing the LMS positions and suffix strings in their original order 
		vector<lmsSubstringBucket> lmsArrayAlpha; //The array storing the LMS suffix strings in alphabetical order 
	//---------------------------------------------------------------------------------------------------
	{
	//Initialize LMS map for O(n) access to indices
	for (size_t i = 0; i < lmsArray.size(); ++i) {
		lmsArrayMap[lmsArray.at(i).index] = lmsArray.at(i);
	}
	
	//Optional Assertion
//	cout << endl << "Now printing LMS map information..." << endl;
//	for (size_t i = 0; i < lmsArrayMap.size(); ++i) {
//		if (lmsArrayMap.at(i).value != -1) {
//			cout << "-----------------------------------------------" << endl;
//			cout << "Information for LMS bucket at [" << i << "]: " << endl;
//			cout << "  Stored string-: " << lmsArrayMap.at(i).bucketName << endl;
//			cout << "  Starting index: " << lmsArrayMap.at(i).index << endl;
//			cout << "  Ending index--: " << lmsArrayMap.at(i).endingIndex << endl;
//		}
//		
//	}
//	cout << endl;
	
	//Collect LMS substrings in alphabetical order
	for (size_t i = 0; i < textLength; ++i) {
		int currIndex = suffArr.at(i);
		
		if (notes.at(currIndex).LMS == true) {
			lmsArrayAlpha.push_back(lmsArrayMap[currIndex]);
		}
	}
	
	//Optional Assertion
//	cout << endl << "Now printing alphabetical LMS information..." << endl;
//	for (size_t i = 0; i < lmsArrayAlpha.size(); ++i) {
//		cout << "-----------------------------------------------" << endl;
//		cout << "Information for LMS bucket " << i << ": " << endl;
//		cout << "  Stored string-: " << lmsArrayAlpha.at(i).bucketName << endl;
//		cout << "  Starting index: " << lmsArrayAlpha.at(i).index << endl;
//		cout << "  Ending index--: " << lmsArrayAlpha.at(i).endingIndex << endl;
//	}
//	cout << endl;
	
	//Initialize name substring map
	int currentName = 0;
	for (size_t i = 0; i < lmsArrayAlpha.size(); ++i) {
		int currentIndex = lmsArrayAlpha.at(i).index;
		
		substringNames[currentIndex] = currentName;
		
		if (i+1 != lmsArrayAlpha.size() && lmsArrayAlpha.at(i).bucketName != lmsArrayAlpha.at(i+1).bucketName) {
			++currentName;
		}
		
	}
	
	//Optional Assertion
//	cout << endl << "Now printing name substring map information..." << endl;
//	cout << "-----------------------------------------------" << endl;
//	for (size_t i = 0; i < textLength; ++i) {
//		if (substringNames[i] != -1) {
//			cout << "Name stored at [" << i << "]: " << substringNames[i] << endl;
//		}
//	}
//	cout << endl;
	}
	//---------------------------------------------------------------------------------------------------
	//===================================================================================================
	
	
	//Step 4: Sort LMS substrings
	//===================================================================================================
		bool hasDuplicates = false; //Whether the reduced string shows that there are two identical LMS bucket names gleaned from the original text
		vector<int> reducedCharCounts (ALPHABET_SIZE, 0); //Number of times each unique character appears in the reduced string
		SuffixArray reducedString; //The reduced string gleaned from the name array
		SuffixArray lmsSuffixArray (ALPHABET_SIZE); //The suffix array for the reduced string
		vector<lmsSubstringBucket> result (lmsArray.size()); //The array resulting from sorting the LMS substrings
	//---------------------------------------------------------------------------------------------------
	//Check for duplicates
	for (size_t i = 0; i < textLength && !hasDuplicates; ++i) {
		if (charCounts[text.at(i)]+1 > 1) { //NOTE: CHANGED FROM INCREMENT TO +1
			hasDuplicates = true;
		}
	}
	
	//Create the reduced string
	for (size_t i = 0; i < substringNames.size(); ++i) {
		if (substringNames[i] != -1) {
			reducedString.push_back(substringNames[i]);
		}
	}
	
	//Optional Assertion
//	cout << endl << "Now printing reduced string information..." << endl;
//	cout << "-----------------------------------------------" << endl;
//	for (size_t i = 0; i < reducedString.size(); ++i) {
//		cout << "Reduced string at [" << i << "]: " << reducedString[i] << endl;
//	}
//	cout << endl;
	
	//Get LMS suffix array
	if (hasDuplicates) {
		lmsSuffixArray = dc3(reducedString);
	} else {
		for (size_t i = 0; i < reducedString.size(); ++i) {
			lmsSuffixArray.at(reducedString[i]) = i;
		}
	}
	
	//Optional Assertion
//	cout << endl << "Now printing resulting LMS suffix array..." << endl;
//	cout << "-----------------------------------------------" << endl;
//	for (size_t i = 0; i < lmsSuffixArray.size(); ++i) {
//		cout << lmsSuffixArray.at(i) << " ";
//	}
//	cout << endl << endl;

	//Map indices to LMS suffix locations
	for (size_t i = 0; i < lmsSuffixArray.size(); ++i) {
		result.at(i) = lmsArray.at(lmsSuffixArray.at(i));
	}
	
	result.resize(lmsArray.size());
	
	//Optional Assertion
//	cout << endl << "Now printing resulting LMS array..." << endl;
//	cout << "-----------------------------------------------" << endl;
//	for (size_t i = 0; i < result.size(); ++i) {
//		cout << result.at(i).index << " ";
//	}
//	cout << endl << endl;
	//---------------------------------------------------------------------------------------------------
	//===================================================================================================
	
	
	//Step 5: Final induced sort
	//===================================================================================================
	//---------------------------------------------------------------------------------------------------
	//Empty the current suffix array
	for (size_t i = 0; i < suffArr.size(); ++i) {
		suffArr.at(i) = -1;
	}
	
	suffArr = inducedSort(suffArr, result, buckets, notes, charCounts, textLength);
	//---------------------------------------------------------------------------------------------------
	//===================================================================================================
	
	
	//Post-processing: Convert int suffix array to size_t
	//===================================================================================================
	//---------------------------------------------------------------------------------------------------
	for (size_t i = 0; i < suffArr.size(); ++i) {
		finalSuffArr.at(i) = suffArr.at(i);
	} 
	//---------------------------------------------------------------------------------------------------
	//===================================================================================================
	
	return finalSuffArr;
}

vector<int> inducedSort(vector<int> &suffArr, vector<lmsSubstringBucket> &lmsArray, vector<bucketInfo> &buckets, vector<annotation> &notes, vector<int> &charCounts, size_t &textLength) {
		//Reset buckets
		createBuckets(buckets, charCounts);
		
		//Reverse pass over LMS
		for (int i = static_cast<int>(lmsArray.size())-1; i >= 0 ; --i) {
			//Find correct bucket
			int letterRank = lmsArray.at(i).value;
			
			//Use bucket to find last free index
			int blockEndIndex = buckets.at(letterRank).emptySlotRight;
			
			//Fill the last free index
			suffArr[blockEndIndex] = lmsArray.at(i).index;
			
			//Update the last free index
			--buckets.at(letterRank).emptySlotRight;
		}
		
		//Optional assertion
//		cout << endl << "Now Printing pass 1 data..." << endl;
//		for (int i = 0; i < static_cast<int>(suffArr.size()); ++i) {
//			cout << suffArr.at(i) << " ";
//		}
//		cout << endl << endl;
		
		//Forward pass over suffix array
		for (size_t i = 0; i < textLength; ++i) {
			int previousIndex = suffArr.at(i)-1;
			
			//If the current index has a value stored, and the number before that stored value is not out of bounds, and the character of the latter value is an L-type
			if (suffArr.at(i) > -1 && previousIndex > -1 && notes.at(previousIndex).type == 'L') {
				//Find correct bucket
				int letterRank = notes.at(previousIndex).value;
				
				//Use bucket to find first free index
				int blockEndIndex = buckets.at(letterRank).emptySlotLeft;
				
				//Fill the last free index
				suffArr[blockEndIndex] = previousIndex;
				
				//Update the last free index
				++buckets.at(letterRank).emptySlotLeft;
				
			}
		}
		
		//Optional assertion
//		cout << endl << "Now Printing pass 2 data..." << endl;
//		for (int i = 0; i < static_cast<int>(suffArr.size()); ++i) {
//			cout << suffArr.at(i) << " ";
//		}
//		cout << endl << endl;
		
		//Reset bucket right slots for final pass
		// for (size_t i = 0; i < buckets.size(); ++i) {
		// 	static int counter = 0; //Keeps track of index sums
			
		// 	//Reset bucket right slot
		// 	buckets.at(i).emptySlotRight = counter + charCounts[i] - 1;
			
		// 	//Increment counter by the number of times the current character appears
		// 	counter += charCounts[i];
		// }

		createBuckets(buckets, charCounts);
		
		
		//Reverse pass over suffix array
		for (int i = static_cast<int>(textLength)-1; i >= 0 ; --i) {
			int previousIndex = suffArr.at(i)-1;
			
			//If the current index has a value stored, and the number before that stored value is not out of bounds, and the character of the latter value is an s-type
			// cout << "Previous index: " << previousIndex << " | Notes value: " << notes.at(previousIndex).type << endl;
			if (suffArr.at(i) > -1 && previousIndex > -1 && notes.at(previousIndex).type == 's') {
				//Find correct bucket
				int letterRank = notes.at(previousIndex).value;
				
				//Use bucket to find first free index
				int blockEndIndex = buckets.at(letterRank).emptySlotRight;
				
				//Fill the last free index
				suffArr[blockEndIndex] = previousIndex;
				
				//Update the last free index
				--buckets.at(letterRank).emptySlotRight;
				
			}
		}
		
		//Optional Assertion
//		cout << endl << "Now Printing pass 3 data..." << endl;
//		for (int i = 0; i < static_cast<int>(suffArr.size()); ++i) {
//			cout << suffArr.at(i) << " ";
//		}
//		cout << endl << endl;
	
	return suffArr;
}

void createBuckets(vector<bucketInfo> &buckets, vector<int> &charCounts) {
	int counter = 0; //Keeps track of index sums
	
	//Create buckets using character counts
	for (size_t i = 0; i < ALPHABET_SIZE; ++i) {
		if (charCounts[i] != 0) {
			//Collect bucket information
			bucketInfo currentBucket;
			currentBucket.bucketName = i;
			currentBucket.startingIndex = counter;
			currentBucket.endingIndex = counter + charCounts[i] - 1;
			currentBucket.emptySlotLeft = currentBucket.startingIndex;
			currentBucket.emptySlotRight = currentBucket.endingIndex;
			
			//Increment counter by the number of times the current character appears
			counter += charCounts[i];
			
			//Save the current bucket to the bucket map
			buckets[i] = currentBucket;
			
			//Optional Assertion
//			cout << "------------------------------------------------" << endl;
//			cout << "Map at " << i << ": " << charCounts[i] << endl;
//			cout << "  Bucket Value------: " << buckets.at(i).bucketName << endl;
//			cout << "  LMS Starting Index: " << buckets.at(i).startingIndex << endl;
//			cout << "  LMS Ending Index--: " << buckets.at(i).endingIndex << endl;
//			cout << "  Empty Slot Left---: " << buckets.at(i).emptySlotLeft << endl;
//			cout << "  Empty Slot Right--: " << buckets.at(i).emptySlotRight << endl;
		}
	}
	
	return;
} 
