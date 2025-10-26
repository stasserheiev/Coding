#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <queue>
#include <BitString.h>
#include <algorithm>
#include <set> //for unique bytes
#include <string>
#include <tuple>
#include <unordered_map>

using namespace std;

struct Node {
	uint8_t num;
	uint32_t freq;
	Node* left;
	Node* right;
	Node* parent;

	Node(uint8_t n, uint32_t f) : num(n), freq(f), left(nullptr), right(nullptr), parent(nullptr) {};
};

struct Compare {
	bool operator()(Node* a, Node* b) const {
		return a->freq > b->freq; // For a min-heap
	}
};

/*Implementing of DFS algorithm to find a specific byte*/
bool findByte(Node* root, uint8_t target, string& out) {
	if (root == nullptr) return false;

	if (!root->left and !root->right) {
		if (root->num == target) return true;
		else return false;
	}
	if (root->left) {
		out = out + "0";
		if(findByte(root->left, target, out)) return true;
		out.pop_back();
	}
	if (root->right) {
		out = out + "1";
		if (findByte(root->right, target, out)) return true;
		out.pop_back();
	}
	return false;
}

void createCodes(Node* root, const string& out, unordered_map<uint8_t, string>& map) {
	if (!root) return;
	if (!root->left and !root->right) {
		map[root->num] = out;
		return;
	}
	createCodes(root->left, out + "0", map);
	createCodes(root->right, out + "1", map);
	return;
}

void freeTree(Node* node) {
	if (!node) return;
	freeTree(node->left);
	freeTree(node->right);
	delete node;
}

tuple<vector<uint8_t>, vector<uint32_t>, uint32_t> HuffmansCoding(vector<uint8_t> bytes) {
	vector<uint32_t> freq(256);
	vector<Node*> node;
	set<uint8_t> unique_bytes;

	/*Save unique bytes in set unique_bytes*/
	for (uint8_t i : bytes) {
		unique_bytes.insert(i);
	}

	/*Count frequencies of input bytes*/
	for (uint8_t i : bytes) {
		freq[i] += 1;
	}

	/*Complete the vector of Nodes*/
	for (uint8_t j : unique_bytes) {
		Node* temp_node = new Node(0, 0);
		temp_node->num = j;
		temp_node->freq = freq[j];
		node.push_back(temp_node);
	}

	/*Creating queue with lowest frequencies as top priority*/
	priority_queue<Node*, vector<Node*>, Compare> pq;
	for (Node* n : node) {
		pq.push(n);
	}

	/*creation of the Huffman's tree*/
	while (pq.size() > 1) {
		Node* left = pq.top();
		pq.pop();
		Node* right = pq.top();
		pq.pop();

		Node* parent = new Node('\0', left->freq + right->freq);
		parent->left = left;
		parent->right = right;
		pq.push(parent);
	}

	Node* root = pq.top();

	/*Generating of Huffman's code*/
	unordered_map<uint8_t, string> HuffmansCodes;
	createCodes(root, "", HuffmansCodes);

	string out = "";
	for (uint8_t i : bytes) {
		out += HuffmansCodes[i];
	}
 

	uint32_t TotalBits = 0;
	vector<uint8_t> coded;
	uint8_t result = 0;
	int num_bits = 0;
	for (char c : out) {
		result = result << 1;
		if (c == '1') {
			result = result | 1;
		}
		num_bits++;

		if (num_bits == 8) {
			coded.push_back(result);
			result = 0;
			num_bits = 0;
		}
		//cout << "Result=" << result;
		TotalBits += 1;
	}

	if (num_bits > 0) {
		coded.push_back(result);
	}
	
	return { coded, freq, TotalBits };
}

vector<uint8_t> HuffmansDecoding(vector<uint32_t> freqs, vector<uint8_t> bytes, uint32_t TotalBits) {
	vector<Node*> node;

	for (int i = 0; i < 256; i++) {
		if (freqs[i] > 0) {
			Node* temp_node = new Node(i, freqs[i]);
			node.push_back(temp_node);
		}
	}

	priority_queue<Node*, vector<Node*>, Compare> pq;
	for (Node* n : node) {
		pq.push(n);
	}

	/*creation of the Huffman's tree*/
	while (pq.size() > 1) {
		Node* left = pq.top();
		pq.pop();
		Node* right = pq.top();
		pq.pop();

		Node* parent = new Node('\0', left->freq + right->freq);
		parent->left = left;
		parent->right = right;
		pq.push(parent);
	}

	Node* root = pq.top();

	Node* temp_node = root;
	vector<uint8_t> result;
	size_t numbits = 0;


	for (uint8_t byte : bytes) {
		for (int i = 7; i >= 0; i--) {
			if (numbits >= TotalBits) break;
			if (((byte >> i) & 1) == 0) {
				temp_node = temp_node->left;
			}
			if (((byte >> i) & 1) == 1) {
				temp_node = temp_node->right;
			}

			if (!temp_node->left && !temp_node->right) {
				result.push_back(temp_node->num);
				temp_node = root;
			}

			numbits++;
		}
	}

	freeTree(root);

	return result;
}

int main() {
	vector<uint8_t> bytes;

	vector<uint8_t> bits;
	vector<uint32_t> freqs;
	int TotalBits = 0;


	std::ifstream file("D:\\lab3_test5.txt", std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file." << endl;
		return {};
	}

	file.seekg(0, std::ios::end);
	streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	vector<uint8_t> outputBytes(size);


	file.read(reinterpret_cast<char*>(outputBytes.data()), size);


	tie(bits, freqs, TotalBits) = HuffmansCoding(outputBytes);
	cout << "Total bits =" << TotalBits << endl;
	

	/*Write frequencies and Huffman's code*/
	ofstream output("D:\\lab3_test5.bin", std::ios::binary);
	if (output.is_open()) {
		output.write(reinterpret_cast<char*>(freqs.data()), freqs.size()*sizeof(uint32_t));
		output.write(reinterpret_cast<char*>(&TotalBits), sizeof(int));
		output.write(reinterpret_cast<char*>(bits.data()), bits.size() * sizeof(uint8_t));
		output.close();
		cout << "Coding is finished!";
	}
	else {
		cerr << "Error opening file." << endl;
	}


	std::ifstream file2("D:\\lab3_test5.bin", std::ios::binary);
	if (!file2.is_open()) {
		std::cerr << "Error: Could not open file." << endl;
		return {};
	}

	vector<uint32_t> freqs_out(256);
	for (uint32_t& f : freqs_out) {
		file2.read(reinterpret_cast<char*>(&f), sizeof(uint32_t));
	}

	uint32_t validBits;
	file2.read(reinterpret_cast<char*>(&validBits), sizeof(uint32_t));
	
	vector<uint8_t> encoded((istreambuf_iterator<char>(file2)), {});

	vector<uint8_t> decoded = HuffmansDecoding(freqs_out, encoded, validBits);



	ofstream output2("D:\\lab3_test5_.txt", std::ios::binary);
	if (output2.is_open()) {

		output2.write(reinterpret_cast<char*>(decoded.data()), decoded.size() * sizeof(uint8_t));
		output2.close();
	}
	else {
		cerr << "Error opening file." << endl;
	}



	cin.get();
	return 0;
}
