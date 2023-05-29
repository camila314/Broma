#pragma once

#include <string>
#include <vector>
#include <variant>
#include <map>
#include <algorithm>
#include <iostream>

namespace broma {
	enum class Platform {
		Mac,
		iOS,
		Windows,
		Android
	};

	struct PlatformNumber {
		size_t mac = 0;
		size_t ios = 0;
		size_t win = 0;
		size_t android = 0;
	};

	struct Type {
		bool is_struct = false;
		std::string name;

		bool operator==(Type const& t) const {
			return name == t.name;
		}
	};

	// The signature of a free function.
	struct FunctionProto {
		Type ret;
		std::vector<std::pair<Type, std::string>> args;
		std::string docs;
		std::string name;

		inline bool operator==(FunctionProto const& f) const {
			if (name != f.name || args.size() != f.args.size()) {
				return false;
			}

			for (size_t i = 0; i < args.size(); ++i) {
				if (!(args[i].first == f.args[i].first)) {
					return false;
				}
			}
			
			return true;
		}
	};


	enum class FunctionType {
		Normal,
		Ctor,
		Dtor
	};

	struct MemberFunctionProto : FunctionProto {
		FunctionType type = FunctionType::Normal;
		bool is_const = false;
		bool is_virtual = false;
		bool is_callback = false;
		bool is_static = false;

		inline bool operator==(MemberFunctionProto const& f) const {
			if (!FunctionProto::operator==(f))
				return false;

			if (is_const != f.is_const)
				return false;

			return true;
		}
	};

	// Member variables.
	struct MemberField {
		std::string name;
		Type type;
		size_t count = 0;
	};

	// Structure padding.
	struct PadField {
		PlatformNumber amount;
	};

	// Function that is bound to an address.
	struct FunctionBindField {
		MemberFunctionProto prototype;
		PlatformNumber binds;
	};

	// Function body that goes in the source.
	struct OutOfLineField {
		MemberFunctionProto prototype;
		std::string inner;
	};

	// Function body that goes in the header.
	struct InlineField {
		std::string inner;
	};

	struct Field {
		size_t field_id;
		std::string parent;
		std::variant<InlineField, OutOfLineField, FunctionBindField, PadField, MemberField> inner;

		template <typename T>
		T* get_as() {
			return std::get_if<T>(&inner);
		}

		template <typename T>
		T const* get_as() const {
			return std::get_if<T>(&inner);
		}

		inline MemberFunctionProto* get_fn() {
			if (auto fn = get_as<OutOfLineField>()) {
				return &fn->prototype;
			} else if (auto fn = get_as<FunctionBindField>()) {
				return &fn->prototype;
			} else return nullptr;
		}
	};

	// Class declaration.
	struct Class {
		std::string name;
		std::vector<std::string> superclasses;
		std::vector<std::string> depends;
		std::vector<Field> fields;

		inline bool operator==(Class const& c) const {
			return name == c.name;
		}
		inline bool operator==(std::string const& n) const {
			return name == n;
		}
	};

	// Free function binding.
	struct Function {
		FunctionProto prototype;
		PlatformNumber binds;
	};

	struct Root {
		std::vector<Class> classes;
		std::vector<Function> functions;

		inline Class* operator[](std::string const& name) {
			auto it = std::find_if(classes.begin(), classes.end(), [name](Class& cls) {
					return cls.name == name;
			});

			if (it == classes.end())
				return nullptr;

			return &*it;
		}
	};
} // namespace broma