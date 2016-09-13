#ifndef tmplORM__HXX
#define tmplORM__HXX

#include <cstdint>
#include <tuple>
#include "typestring/typestring.hh"

namespace tmplORM
{
	using namespace irqus;

	template<typename _tableName> struct model_t
	{
		const char *tableName() const noexcept { return _tableName::data(); }

		// create(); - Creates the table
		// add(); - CRUD Create
		// update(); - CRUD Update
		// delete(); - CRUD Delete
	};

	namespace types
	{
		template<typename _fieldName, typename T> struct field_t
		{
			T _value;
			bool _modified;

			field_t() noexcept : _value(), _modified(false) { }

			constexpr const char *fieldName() const noexcept { return _fieldName::data(); }
			const T value() const noexcept { return _value; }
			// Make the type behave like it's contained type..
			operator T() const noexcept { return _value; }
			bool modified() const noexcept { return _modified; }

			void value(const T &newValue) noexcept
			{
				_value = newValue;
				_modified = true;
			}

			// create(); - CREATE TABLE definiton of the field
		};

		template<typename _fieldName> struct int64_t final : public field_t<_fieldName, std::int64_t>
		{
		};

		template<typename _fieldName> struct int32_t final : public field_t<_fieldName, std::int32_t>
		{
		};

		template<typename _fieldName> struct int16_t final : public field_t<_fieldName, std::int16_t>
		{
		};

		template<typename _fieldName> struct int8_t final : public field_t<_fieldName, std::int8_t>
		{
		};

		template<typename _fieldName, uint32_t _length> struct unicode_t final : public field_t<_fieldName, char *>
		{
			constexpr uint32_t length() const noexcept { return _length; }
		};

		// Convinience just in case you don't like using the stdint.h like types above.
		template<typename fieldName> using bigInt_t = int64_t<fieldName>;
		template<typename fieldName> using long_t = int64_t<fieldName>;
		template<typename fieldName> using int_t = int32_t<fieldName>;
		template<typename fieldName> using short_t = int16_t<fieldName>;
		template<typename fieldName> using tinyInt_t = int8_t<fieldName>;
	}

	template<typename... Fields> struct fields_t
	{
	protected:
		constexpr static const size_t N = sizeof...(Fields);
		std::tuple<Fields...> _fields;

		constexpr fields_t() noexcept : _fields{} {}
		constexpr fields_t(Fields &&...fields) noexcept : _fields{fields...} { }
	};
}

#endif /*tmplORM__HXX*/
