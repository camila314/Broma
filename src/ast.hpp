#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <iostream>

using namespace std; // horrific

struct ClassDefinition;

struct ClassField {
	size_t index;
	ClassDefinition* parent_class;

	virtual ~ClassField() {}
};

enum FunctionType {
	kVirtualFunction=0,
	kStaticFunction=1,
	kRegularFunction=2,
	//kStructorCutoff=10, // used for comparisons
	kConstructor=11,
	kDestructor=12
};

struct Function : ClassField {
	bool is_const;
	FunctionType function_type;

	string return_type;
	string name;
	vector<string> args;

	string binds[3]; // mac, windows, ios (android has all symbols included). No binding = no string. Stored as a string because no math is done on it
	string android_mangle; // only sometimes matters. empty if irrelevant
};

struct Member : ClassField {
	string type;
	string name;
	bool hardcode;
	string hardcodes[3]; // mac/ios, windows, android
	size_t count; // for arrays
};

struct Inline : ClassField {
	string inlined;
};

struct Root;
struct ClassDefinition {
	string name;
	vector<string> superclasses;
	vector<size_t> function_indexes;
	vector<size_t> member_indexes;
	vector<size_t> inline_indexes;
	vector<char[sizeof Function]> in_order; //lol xd

	void addSuperclass(string sclass) {
		if (std::find(superclasses.begin(), superclasses.end(), sclass) != superclasses.end()) {
			cacerr("Duplicate superclass %s for class %s\n", sclass.c_str(), name.c_str());
		}
		superclasses.push_back(sclass);
	}
};

struct Root {
	map<string, ClassDefinition> classes;

	ClassDefinition& addClass(string name) {
		classes[name] = ClassDefinition();
		classes[name].name = name;
		return classes[name];
	}
};
