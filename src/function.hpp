#pragma once

#include <tao/pegtl.hpp>
using namespace tao::pegtl;

#include "basic_components.hpp"
#include "attribute.hpp"
#include "type.hpp"
#include "bind.hpp"
#include "state.hpp"

namespace broma {
	/// @brief A function signature / prototype.
	struct function_proto :
		seq <rule_begin<function_proto>,
			opt<attribute>,
			sep,
			tagged_rule<function_proto, type>,
			sep,
			tagged_rule<function_proto, identifier>,
			arg_list>
		{};

	template <>
	struct run_action<rule_begin<function_proto>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->is_class = false;
			scratch->wip_fn_proto = FunctionProto();
		}
	};

	template <>
	struct run_action<tagged_rule<function_proto, type>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto.ret = scratch->wip_type;
		}
	};

	template <>
	struct run_action<tagged_rule<function_proto, identifier>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto.name = input.string();
		}
	};

	/// @brief A full function (prototype and bindings).
	struct function : 
		seq<rule_begin<function>, function_proto, sep, bind>
		{};

	template <>
	struct run_action<function> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			Function f;
			f.prototype = scratch->wip_fn_proto;
			f.binds = scratch->wip_bind;
			root->functions.push_back(f);
		}
	};

	/// @brief A member function prototype.
	///
	/// This allows some more qualifiers than the free function prototype.
	struct member_function_proto :
		seq<rule_begin<member_function_proto>, opt<attribute>, sep, sor<
			// ctor, dtor
			seq<
				named_rule("structor", success),
				opt<pad_space<keyword_virtual>>,
				opt<tagged_rule<member_function_proto, one<'~'>>>,
				tagged_rule<member_function_proto, identifier>,
				arg_list
			>,

			// not ctor, dtor
			if_then_must<
				sor<keyword_static, keyword_virtual, keyword_callback>,
				named_rule("fn_member", success),
				sep,
				tagged_rule<member_function_proto, type>,
				sep,
				tagged_rule<member_function_proto, identifier>,
				arg_list,
				sep,
				opt<tagged_rule<member_function_proto, keyword_const>>
			>
		>> {};

	template <>
	struct run_action<rule_begin<member_function_proto>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_mem_fn_proto = MemberFunctionProto();
		}
	};

	template <>
	struct run_action<member_function_proto> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			for (auto& f : scratch->wip_class.fields) {
				if (auto fn = f.get_fn()) {
					if (*fn == scratch->wip_mem_fn_proto) {
						scratch->errors.push_back(parse_error("Function duplicate!", input.position()));
					}
				}
			}
		}
	};

	template <>
	struct run_action<tagged_rule<member_function_proto, identifier>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_mem_fn_proto.name = input.string();

			if (scratch->wip_mem_fn_proto.type == FunctionType::Dtor)
				scratch->wip_mem_fn_proto.name = "~" + scratch->wip_mem_fn_proto.name;
		}
	};

	template <>
	struct run_action<tagged_rule<member_function_proto, keyword_const>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_mem_fn_proto.is_const = true;
		}
	};

	template <>
	struct run_action<tagged_rule<member_function_proto, type>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_mem_fn_proto.ret = scratch->wip_type;
		}
	};

	template <>
	struct run_action<named_rule("fn_member", success)> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_mem_fn_proto.type = FunctionType::Normal;
		}
	};

	template <>
	struct run_action<named_rule("structor", success)> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_mem_fn_proto.type = FunctionType::Ctor;
		}
	};

	template <>
	struct run_action<keyword_static> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_mem_fn_proto.is_static = true;
		}
	};

	template <>
	struct run_action<tagged_rule<member_function_proto, one<'~'>>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_mem_fn_proto.type = FunctionType::Dtor;
		}
	};

	template <>
	struct run_action<keyword_virtual> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_mem_fn_proto.is_virtual = true;
		}
	};

	template <>
	struct run_action<keyword_callback> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_mem_fn_proto.is_callback = true;
		}
	};

	/// @brief A member function body (should go in a .cpp file).
	struct ool_expr : seq<member_function_proto, sep, tagged_rule<ool_expr, brace_start>> {};

	template <>
	struct run_action<tagged_rule<ool_expr, brace_start>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			OutOfLineField f;
			f.prototype = scratch->wip_mem_fn_proto;
			f.inner = input.string();
			scratch->wip_field.inner = f;
		}
	};

	/// @brief An offset binding expression.
	struct bind_expr : seq<member_function_proto, sep, bind> {};

	template <>
	struct run_action<bind_expr> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			FunctionBindField f;
			f.prototype = scratch->wip_mem_fn_proto;
			f.binds = scratch->wip_bind;
			scratch->wip_field.inner = f;
		}
	};
} // namespace broma
