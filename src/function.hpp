#pragma once

#include <tao/pegtl.hpp>
using namespace tao::pegtl;

#include "basic_components.hpp"
#include "attribute.hpp"
#include "type.hpp"
#include "bind.hpp"
#include "state.hpp"

namespace broma {
	struct function_proto :
		seq<rule_begin<function_proto>, opt<attribute>, sep, sor<
			// ctor, dtor
			seq<
				named_rule("structor", success),
				opt<pad_space<keyword_virtual>>,
				opt<tagged_rule<function_proto, one<'~'>>>,
				tagged_rule<function_proto, identifier>,
				arg_list
			>,

			// not ctor, dtor
			if_then_must<
				sor<keyword_static, keyword_virtual, keyword_callback>,
				named_rule("fn_normal", success),
				sep,
				tagged_rule<function_proto, type>,
				sep,
				tagged_rule<function_proto, identifier>, 
				arg_list,
				sep,
				opt<tagged_rule<function_proto, keyword_const>>
			>
		>> {};

	template <>
	struct run_action<rule_begin<function_proto>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto = FunctionProto();
		}
	};

	template <>
	struct run_action<function_proto> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			for (auto& f : scratch->wip_class.fields) {
				if (auto fn = f.get_fn()) {
					if (*fn == scratch->wip_fn_proto) {
						scratch->errors.push_back(parse_error("Function duplicate!", input.position()));
					}
				}
			}
		}
	};

	template <>
	struct run_action<tagged_rule<function_proto, identifier>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto.name = input.string();

			if (scratch->wip_fn_proto.type == FunctionType::Dtor)
				scratch->wip_fn_proto.name = "~" + scratch->wip_fn_proto.name;
		}
	};

	template <>
	struct run_action<tagged_rule<function_proto, keyword_const>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto.is_const = true;
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
	struct run_action<named_rule("fn_normal", success)> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto.type = FunctionType::Normal;
		}
	};

	template <>
	struct run_action<named_rule("structor", success)> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto.type = FunctionType::Ctor;
		}
	};

	template <>
	struct run_action<keyword_static> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto.is_static = true;
		}
	};

	template <>
	struct run_action<tagged_rule<function_proto, one<'~'>>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto.type = FunctionType::Dtor;
		}
	};

	template <>
	struct run_action<keyword_virtual> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto.is_virtual = true;
		}
	};

	template <>
	struct run_action<keyword_callback> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_fn_proto.is_callback = true;
		}
	};

	struct ool_expr : seq<function_proto, sep, tagged_rule<ool_expr, brace_start>> {};

	template <>
	struct run_action<tagged_rule<ool_expr, brace_start>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			OutOfLineField f;
			f.prototype = scratch->wip_fn_proto;
			f.inner = input.string();
			scratch->wip_field.inner = f;
		}
	};

	struct bind_expr : seq<function_proto, sep, bind> {};

	template <>
	struct run_action<bind_expr> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			FunctionBindField f;
			f.prototype = scratch->wip_fn_proto;
			f.binds = scratch->wip_bind;
			scratch->wip_field.inner = f;
		}
	};
} // namespace broma