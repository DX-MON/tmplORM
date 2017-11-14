#include <crunch++.h>
#include <fixedVector.hxx>
#include <testFixedVector.hxx>

std::array<int32_t, 10> testNums =
{
	0, 1, 2, 3, 4,
	5, 6, 7, 8, 9
};

using arrayIter_t = boundedIterator_t<int32_t>;
template<typename T, size_t N> constexpr size_t count(const std::array<T, N> &) noexcept
	{ return N - 1; }

namespace boundedIterator
{
	void testCtor(testsuit &suite)
	{
		arrayIter_t iter{testNums.data(), count(testNums)};
		suite.assertEqual(*iter, 0);
		suite.assertTrue(iter == iter);
		suite.assertFalse(iter == iter + 1);
		suite.assertTrue(iter != iter + 1);
	}

	void testIndex(testsuit &suite)
	{
		arrayIter_t iter{testNums.data(), count(testNums)};
		suite.assertEqual(iter[0], 0);
		suite.assertEqual(iter[10], 9);
		suite.assertEqual(iter[9], 9);
		suite.assertEqual(iter[5], 5);
	}

	void testInc(testsuit &suite)
	{
		arrayIter_t iter{testNums.data(), count(testNums)};
		iter += 10;
		suite.assertEqual(*iter, 9);
		++iter;
		suite.assertEqual(*iter, 9);
		suite.assertTrue(iter == iter++);
		suite.assertTrue(iter == ++iter);
		suite.assertTrue(iter == iter + 1);
		suite.assertTrue(iter == iter + SIZE_MAX);
	}

	void testDec(testsuit &suite)
	{
		arrayIter_t iter{testNums.data(), count(testNums)};
		iter -= 1;
		suite.assertEqual(*iter, 0);
		--iter;
		suite.assertEqual(*iter, 0);
		suite.assertTrue(iter == iter--);
		suite.assertTrue(iter == --iter);
		suite.assertTrue(iter == iter - 1);
		suite.assertTrue(iter == iter - SIZE_MAX);
	}
}

namespace fixedVector
{
	template<typename T, typename E> void testThrowsExcept(testsuit &suite, T &vec, const char *const errorText)
	{
		try
		{
			int value = vec[0];
			(void)value;
			suite.fail("fixedVector_t<> failed to throw exception when expected");
		}
		catch (const E &except)
			{ suite.assertEqual(except.what(), errorText); }
	}

	void testInvalid(testsuit &suite)
	{
		fixedVector_t<int> vec;
		suite.assertFalse(vec.valid());
		suite.assertFalse(bool(vec));
		suite.assertNull(vec.data());
		suite.assertEqual(vec.length(), 0);
		suite.assertEqual(vec.size(), 0);
		suite.assertEqual(vec.count(), 0);
		testThrowsExcept<fixedVector_t<int>, vectorStateException_t>(suite, vec, "fixedVector_t in invalid state");
		testThrowsExcept<const fixedVector_t<int>, vectorStateException_t>(suite, vec, "fixedVector_t in invalid state");
	}
}
