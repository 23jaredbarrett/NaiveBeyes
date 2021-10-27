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
#include <time.h>
#include <stdio.h>
using namespace std;
// namespaces to make code more readable
using WordCount = unordered_map<string, int>;
using WordProb = unordered_map<string, double>;
// wrap our global variables to avoid pollution
namespace wd {

	// global variables we will be using throughout this entire program
	int spam = 0, ham = 0;
	WordProb spamProb;
	WordProb hamProb;
}

/*
* This method removes all punctuation from a given string
* and converts to lowercase so there aren't different variations of identical words
* 
*/
void removePunctuation(string& line) {
	// this for loop will check if it's punctuation remove it
	// if the letter is not punctuation, convert to lower case
	for (int i = 0, len = line.size(); i < len; i++)
	{
		// check whether parsing character is punctuation or not
		// ispunct causes an error! !isalpha(line[i]) ||
		if ( ispunct(static_cast<unsigned char>(line[i]))) {
			line.erase(i--, 1);
			len = line.size();
		} else {
			// convert to  lower case
			line[i] = tolower(line[i]);
		}
	}
	// Remove punctuations in a line
	// line.erase(std::remove_if(line.begin(), line.end(), ispunct), line.end());
	// std::replace_if(line.begin(), line.end(), ::ispunct, ' ');
	// convert to lower case to ensure we can check against our map
	// std::transform(line.begin(), line.end(), line.begin(), ::tolower);
}
/**
* This method is used to count each unique word within the given email
* Also, the method removes punctuation from the words (which may not be necessary)
* \param[in] wc spamCount or wordCount
* \param[in] email the given email we are going to parse for certain words
*/
void getWordCount(WordCount& wc, string& email) {
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
* 
* 
* \param[in] in file we are using to 
*/
void calcGivens(ifstream& in) {
	string sOrH, email, line;
	in >> sOrH >> sOrH;
	WordCount spamCount;
	WordCount hamCount;
	// we're reading file wrong
	// in >> quoted(sOrH, ',') >> quoted(email, '\n')
	while (getline(in, line)) {
		
		// create set of words within this particular email
		if (line.substr(0, 3) == "ham") {
			email = line.substr(4);
			wd::ham++;
			getWordCount(hamCount, email);
		} else {
			email = line.substr(5);
			wd::spam++;
			getWordCount(spamCount, email);
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
		wd::hamProb[wordHa.first] = (wordHa.second + 1.0) / (wd::ham + 2.0);
		// handle edge case where word is located in ham but not spam
		wd::spamProb[wordHa.first] = 1.0/ (2.0 + wd::spam);
	}
	for (auto& wordSp : spamCount) {
		
		wd::spamProb[wordSp.first] = (wordSp.second+1.0) / (wd::spam+2);
		// handle edge case wheere word is located in spam but not ham
		if (wd::hamProb.find(wordSp.first) == wd::hamProb.end()) {
			wd::hamProb[wordSp.first] = 1.0 / (2.0 + wd::ham);
		}
	}
}

double calcProbabilitySpam(std::string& email) {
	removePunctuation(email);
	// use string stream to loop trough words and make set that is contained
	// in our training set
	set<string> words;
	stringstream ss(email);
	string word;
	// first, create set of words contained both within this email and training set
	while (ss >> word) {
		// we only need to check one ham set or spam set to check
		// if the word is in it because earlier, we ensured all words within hamProb are in spamProb
		if ((wd::hamProb.find(word) != wd::hamProb.end())) {
			words.insert(word);
		}
	} 
	double probOfSpam = static_cast<double>(wd::spam) / (wd::spam + wd::ham);
	double probOfHam = static_cast<double>(wd::ham) / (wd::spam + wd::ham);
	// we need three for loops for different products:
	// product of probability of each word given the email is spam
	// product of probability of each word given the email is ham
	double probWordsGivSpam = 1.0;
	double probWordsGivHam = 1.0;
	for (const auto& word : words) {
		probWordsGivHam *= wd::hamProb.at(word);
	}
	for (const auto& word : words) {
		probWordsGivSpam *= wd::spamProb.at(word);
	}
	// create a set of unqiue words which are in our training set
	return (probOfSpam*probWordsGivSpam) / ((probOfSpam * probWordsGivSpam) + (probOfHam * probWordsGivHam));
}

int main(int argc, char** argv) {

	// if argc > 1, we have not trained our dataset yet!
	// if (argc > 1) {
		ifstream trainFile("spam.csv");
		if (!trainFile.good()) return 404;
		time_t start = time(0);
		calcGivens(trainFile);
		double seconds_since_start = difftime(time(0), start);
		cout << "data trained properly in " << seconds_since_start << " seconds." << endl;
	/* }
	else {
		cout << "no data has been saved yet" << endl;
		return 0;
	}
	*/
	string input;
	cout << "Enter emails! type \"exit\" or \"q\" to quit" << endl;
	/*
	* 
	*/
	while (std::getline(cin, input)) {
		if (input == "end" || input == "q") {
			break;
		}
		double emailSpam = calcProbabilitySpam(input);
		cout << "Probability this email is spam is: " <<
			emailSpam << "\nthis email is " << ((emailSpam > 0.5) ? "SPAM" : "HAM") << std::endl <<std::endl;
	}
}