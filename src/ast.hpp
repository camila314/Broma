#pragma once

#include <string>
#include <vector>
#include <unordered_map>
using namespace std; // horrific

struct ClassField {
	size_t index;
};

struct Function : ClassField {
	bool is_virtual;
	bool is_static;
	bool is_const;

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
	vector<Function> functions;
	vector<Member> members;
	vector<Inline> inlines;
	vector<ClassField*> in_order;

	void addSuperclass(string sclass) {
		if (std::find(superclasses.begin(), superclasses.end(), sclass) != superclasses.end()) {
			cacerr("Duplicate superclass %s for class %s\n", sclass.c_str(), name.c_str());
		}
		superclasses.push_back(sclass);
	}
};

struct Root {
	unordered_map<string, ClassDefinition> classes;
	vector<string> parsed_classes; // to be used differently per method

	ClassDefinition& addClass(string& name) {
		classes[name] = ClassDefinition();
		classes[name].name = name;
		return classes[name];
	}
};
