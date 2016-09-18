#ifndef tmplORM_MYSQL__HXX
#define tmplORM_MYSQL__HXX

#include "tmplORM.hxx"

namespace tmplORM
{
	namespace mysql
	{
		using tmplORM::types::type_t;
		using tmplORM::model_t;

		template<typename name> using backtick = tycat<ts("`"), name, ts("`")>;

		template<size_t, typename> struct fieldName_t { };
		template<size_t N, typename fieldName, typename T> struct fieldName_t<N, type_t<fieldName, T>>
			{ using value = tycat<backtick<fieldName>, ts(", ")>; };
		template<typename fieldName, typename T> struct fieldName_t<1, type_t<fieldName, T>> { using value = backtick<fieldName>; };

		template<size_t N> struct selectList__t
		{
			template<typename fieldName, typename T> constexpr static auto value(const type_t<fieldName, T> &) ->
				typename fieldName_t<N, type_t<fieldName, T>>::value;
		};
		template<size_t N, typename T> using selectList__ = decltype(selectList__t<N>::value(T()));

		template<size_t N, typename field, typename... fields> struct selectList_t
			{ using value = tycat<selectList__<N, field>, typename selectList_t<N - 1, fields...>::value>; };
		template<typename field> struct selectList_t<1, field> { using value = selectList__<1, field>; };

		template<typename... fields> using selectList = typename selectList_t<sizeof...(fields), fields...>::value;

		template<typename tableName, typename... fields> using select__ = tycat<ts("SELECT "), selectList<fields...>,
			ts(" FROM "), backtick<tableName>, ts(";")>;
		template<typename tableName, typename... fields> bool select_(const model_t<tableName, fields...> &) noexcept
		{
			using select = select__<tableName, fields...>;
			return true;
		}
		template<typename... models> bool select() noexcept { return collect(select_(models())...); }
	};
};

#endif /*tmplORM_MYSQL__HXX*/
