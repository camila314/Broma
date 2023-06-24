#pragma once

#include <string>
#include <vector>
#include <variant>
#include <map>
#include <algorithm>
#include <iostream>

namespace broma {
	/// @brief The platform currently being processed in a bind statement.
	enum class Platform {
		Mac,
		iOS,
		Windows,
		Android
	};

	/// @brief Binding offsets for each platform.
	struct PlatformNumber {
		size_t mac = 0;
		size_t ios = 0;
		size_t win = 0;
		size_t android = 0;
	};

	/// @brief A C++ type declaration.
	struct Type {
		bool is_struct = false;
		std::string name;

		bool operator==(Type const& t) const {
			return name == t.name;
		}
	};

	/// @brief The signature of a free function.
	struct FunctionProto {
		Type ret; ///< The return type of the function.
		std::vector<std::pair<Type, std::string>> args; ///< All arguments, represented by their type and their name.
		std::string docs; ///< Any docstring pulled from a `[[docs(...)]]` attribute.
		std::string name; ///< The function's name.

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

	/// @brief A member function's type.
	enum class FunctionType {
		Normal,
		Ctor, ///< A constructor.
		Dtor ///< A destructor.
	};

	/// @brief The signature of a member function.
	struct MemberFunctionProto : FunctionProto {
		FunctionType type = FunctionType::Normal;
		bool is_const = false;
		bool is_virtual = false;
		bool is_callback = false; ///< Whether or not this function is a callback.
								  ///< In Geode, this determines the function's calling convention (optcall or thiscall).
		bool is_static = false;

		inline bool operator==(MemberFunctionProto const& f) const {
			if (!FunctionProto::operator==(f))
				return false;

			if (is_const != f.is_const)
				return false;

			return true;
		}
	};

	/// @brief A class's member variables.
	struct MemberField {
		std::string name;
		Type type;
		size_t count = 0;
	};

	/// @brief Any class padding.
	struct PadField {
		PlatformNumber amount; ///< The amount of padding, separated per platform.
	};

	/// @brief A function that is bound to an offset.
	struct FunctionBindField {
		MemberFunctionProto prototype;
		PlatformNumber binds; ///< The offsets, separated per platform.
	};

	/// @brief An inline function body that should go in a source file (.cpp).
	struct OutOfLineField {
		MemberFunctionProto prototype;
		std::string inner; ///< The inline body of the function as a raw string.
	};

	/// @brief A inline function body that should go in a header file (.hpp).
	struct InlineField {
		std::string inner; ///< The inline body of the function as a raw string.
	};

	/// @brief A class field.
	struct Field {
		size_t field_id; ///< The index of the field. This starts from 0 and counts up across all classes.
		std::string parent; ///< The name of the parent class.
		std::variant<InlineField, OutOfLineField, FunctionBindField, PadField, MemberField> inner;

		/// @brief Cast the field into a variant type. This is useful to extract data from the field.
		template <typename T>
		T* get_as() {
			return std::get_if<T>(&inner);
		}

		/// @brief Cast the field into a variant type. This is useful to extract data from the field.
		template <typename T>
		T const* get_as() const {
			return std::get_if<T>(&inner);
		}

		/// @brief Convenience function to get the function prototype of the field, if the field is a function of some sort.
		inline MemberFunctionProto* get_fn() {
			if (auto fn = get_as<OutOfLineField>()) {
				return &fn->prototype;
			} else if (auto fn = get_as<FunctionBindField>()) {
				return &fn->prototype;
			} else return nullptr;
		}
	};

	/// @brief A top-level class declaration.
	struct Class {
		std::string name; ///< The name of the class.
		std::vector<std::string> superclasses; ///< Parent classes that the current class inherits.
		std::vector<std::string> depends; ///< Classes the current class depends on.
										  ///< This includes parent classes, and any classes declared in a `[[depends(...)]]` attribute.
		std::vector<Field> fields; ///< All the fields parsed in the class.

		inline bool operator==(Class const& c) const {
			return name == c.name;
		}
		inline bool operator==(std::string const& n) const {
			return name == n;
		}
	};

	/// @brief A top-level free function binding.
	struct Function {
		FunctionProto prototype; ///< The free function's signature.
		PlatformNumber binds; ///< The offsets of free function, separated per platform.
	};

	/// @brief Broma's root grammar (the root AST).
	///
	/// See the user's guide for an example on how to traverse this AST.
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
