/*
* Implementation of Naive Beyes Algorithm in order
* to calculate the probabilities that a given email
* is spam or not
* Author: Jared Barrett
* Copyright <2021> [Jared Barrett]
*/
#include <vector>
#include <iostream>
#include <fstream> 
#include <unordered_map>
#include <iomanip>
#include <set>
#include <sstream>
#include <algorithm>
using namespace std;
using WordCount = unordered_map<string, int>;
using WordProb = unordered_map<string, double>;
int spam = 0, ham = 0;
WordProb spamProb;
WordProb hamProb;
// global variables we will be using throughout this algorithm

/*
* This method removes all punctuation from a given string
* and converts to lowercase so there aren't a lot of variations of words
* 
*/
void removePunctuation(string& line) {
	// Remove punctuations in a line
	std::replace_if(line.begin(), line.end(), ::ispunct, ' ');
	// convert to lower case to ensure we can check against our map
	std::transform(line.begin(), line.end(), line.begin(), ::tolower);
}
/**
* This method is used to count each unique word within the given email
* Also, the method removes punctuation from the words (which may not be necessary)
* \param[in] wc spamCount or wordCount
* \param[in] email the given email we are going to parse for certain words
*/
void getWordCount(WordCount& wc, string& email) {
	// TODO: NEED TO ADD TO OTHER WORDCOUNT IF NOT EXISTS
	// to handle zeros
	// Remove punctuation from email first
	// by parsing each individual letter
	removePunctuation(email);

	// set used to keep track of words so that if the word occurs twice,
	// we do not count it twice
	set<string> emailWords;
	// send string stream to helper method, returning count
	stringstream ss(email);
	string word;
	while (ss >> word) {
		// if the word is not contained in our set:
		// add it to our set and add it to the spam/hamCount
		if(emailWords.find(word) == emailWords.end()) {
			emailWords.insert(word);
			wc[word]++;
		}
	}

}
/**
* \param[in] in file we are using to 
*/
std::tuple<WordProb, WordProb> calcGivens(ifstream& in) {
	string sOrH, email;
	in >> sOrH >> email;
	WordCount spamCount;
	WordCount hamCount;
	while (in >> quoted(sOrH) >> quoted(email)) {
		// create set of words within this particular email
		if (sOrH == "spam") {
			spam++;
			getWordCount(spamCount, email);
		}
		else {
			ham++;
			getWordCount(hamCount, email);
		}
	}
	// now, we calculate the probabilities:
	// p(word | spam) = num spam emails containing word over totalspam+2
	// p(word | ham) = num ham emails containing word over totalham + 2
	// the reason for adding 1/2 is because if a word is located in this set
	// but not in hamProb, then when we calculate the products of probabilities
	// the total will be zero automatically, and the email would be classified as
	// either 100% spam or 100% not spam unintentionally.
	for (auto& wordHa : hamCount) {
		hamProb[wordHa.first] = (wordHa.second + 1) / (ham + 2);
		// handle edge case where word is located in ham but not spam
		spamProb[wordHa.first] = 1/ (2 + spam);
	}
	for (auto& wordSp : spamCount) {
		
		spamProb[wordSp.first] = (wordSp.second+1) / (spam+2);
		// handle edge case wheere word is located in spam but not ham
		if (hamProb.find(wordSp.first) == hamProb.end()) {
			hamProb[wordSp.first] = 1 / (2 + ham);
		}
	}

	// return these probabilities
	return { spamProb, hamProb };
}

double calcProbabilitySpam(std::string& email) {
	// create a set of unqiue words which are in our training set
	return -1.0;
}

int main(int argc, char** argv) {

	// if argc > 1, we have not trained our dataset yet!
	if (argc > 1) {
		ifstream trainFile(argv[1]);
		if (!trainFile.good()) return 404;

		std::tie(spamProb, hamProb) = calcGivens(trainFile, spam, ham);
		cout << "data trained properly" << endl;
	}
	else {
		cout << "no data has been saved yet" << endl;
		return 0;
	}
	string input;
	cout << "Enter emails! type \"exit\" or \"q\" to quit" << endl;
	/*
	* 
	*/
	while (std::getline(cin, input)) {
		if (input == "end" || input == "q") {
			break;
		}
		removePunctuation(input);
		calcProbabilitySpam(input);
	}
}