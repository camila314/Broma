#pragma once

#include <tao/pegtl.hpp>
using namespace tao::pegtl;

#include "attribute.hpp"
#include "basic_components.hpp"
#include "function.hpp"
#include "member.hpp"

namespace broma {
	/// @brief An inline function body.
	struct inline_expr :
		if_must<
			keyword_inline,
			if_then_else<
				at<rematch<until<eolf>, until<one<'{'>>>>,
				until<brace_start>,
				until<eolf>
			>
		> {};

	template <>
	struct run_action<inline_expr> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			InlineField inf;
			inf.inner = input.string();

			scratch->wip_field.inner = inf;
		}
	};

	struct field : seq<opt<attribute>, sor<inline_expr, pad_expr, member_expr, bind_expr, ool_expr>> {};

	template <>
	struct run_action<field> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			// This happens after the inner expression is done parsing
			scratch->wip_field.parent = scratch->wip_class.name;
			static size_t index = 0;
			scratch->wip_field.field_id = index++;
			scratch->wip_field.links = scratch->wip_link_platform;
			scratch->wip_field.missing = scratch->wip_missing_platform;
			scratch->wip_class.fields.push_back(scratch->wip_field);

			scratch->wip_link_platform = scratch->wip_class.links;
			scratch->wip_missing_platform = scratch->wip_class.missing;
		}
	};

	/// @brief A class declaration.
	struct class_statement :
	seq<
		rule_begin<class_statement>,
		tagged_rule<class_statement, opt<attribute>>,
		sep,
		keyword_class,
		whitespace,
		named_rule("class name", qualified),
		sep,
		opt<
			one<':'>,
			sep,
			list<seq<
				sep,
				named_rule("superclass", qualified),
				sep
			>, one<','>>
		>,
		one<'{'>,
		sep,
		until<one<'}'>, sep, must<field>, sep>
	> {};

	template <>
	struct run_action<named_rule("superclass", qualified)> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			if (scratch->wip_class.name == input.string())
				throw parse_error("Class subclasses itself", input.position());

			scratch->wip_class.superclasses.push_back(input.string());
			scratch->wip_class.depends.push_back(input.string());
		}
	};

	template <>
	struct run_action<named_rule("class name", qualified)> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_class.name = input.string();

			if (std::find(root->classes.begin(), root->classes.end(), input.string()) != root->classes.end()) {
				scratch->errors.push_back(parse_error("Class duplicate! " + input.string(), input.position()));
			}
		}
	};


	template <>
	struct run_action<class_statement> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			root->classes.push_back(std::move(scratch->wip_class));
			//std::cout << "class end\n";
			scratch->wip_class = Class();
			scratch->wip_link_platform = Platform::None;
			scratch->wip_missing_platform = Platform::None;
		}
	};

	template <>
	struct run_action<rule_begin<class_statement>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->is_class = true;
		}
	};

	template <>
	struct run_action<tagged_rule<class_statement, opt<attribute>>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_class.links = scratch->wip_link_platform;
			scratch->wip_class.missing = scratch->wip_missing_platform;
		}
	};
} // namespace broma
