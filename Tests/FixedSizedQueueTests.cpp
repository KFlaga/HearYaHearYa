#include "stdafx.h"
#include "CppUnitTest.h"

#include <FixedSizeQueue.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	using namespace eave;

	TEST_CLASS(FixedSizedQueueTests)
	{
	public:
		TEST_METHOD(OneElementMax)
		{
			FixedSizeQueue<int> queue{ 1 };

			Assert::AreEqual(0, int(queue.size()));
			Assert::AreEqual(1, int(queue.maxSize()));

			queue.push(5);

			Assert::AreEqual(1, int(queue.size()));
			Assert::AreEqual(1, int(queue.maxSize()));
			Assert::AreEqual(5, queue[0]);

			queue.push(3);

			Assert::AreEqual(1, int(queue.size()));
			Assert::AreEqual(1, int(queue.maxSize()));
			Assert::AreEqual(5, queue[0]);

			queue.push(10);

			Assert::AreEqual(1, int(queue.size()));
			Assert::AreEqual(1, int(queue.maxSize()));
			Assert::AreEqual(10, queue[0]);
		}

		TEST_METHOD(ThreeElementsMax)
		{
			FixedSizeQueue<int> queue{ 3 };

			Assert::AreEqual(0, int(queue.size()));
			Assert::AreEqual(3, int(queue.maxSize()));

			queue.push(4);
			queue.push(6);

			Assert::AreEqual(2, int(queue.size()));
			Assert::AreEqual(3, int(queue.maxSize()));
			Assert::AreEqual(6, queue[0]);
			Assert::AreEqual(4, queue[1]);

			queue.push(3);
			queue.push(10);
			queue.push(12);
			queue.push(5);

			Assert::AreEqual(3, int(queue.size()));
			Assert::AreEqual(3, int(queue.maxSize()));
			Assert::AreEqual(12, queue[0]);
			Assert::AreEqual(10, queue[1]);
			Assert::AreEqual(6, queue[2]);
		}

		TEST_METHOD(CustomComparer)
		{
			FixedSizeQueue<int, std::greater<int>> queue{ 3, std::greater<int>{} };

			queue.push(3);
			queue.push(6);
			queue.push(5);
			queue.push(4);
			queue.push(7);

			Assert::AreEqual(3, int(queue.size()));
			Assert::AreEqual(3, int(queue.maxSize()));
			Assert::AreEqual(3, queue[0]);
			Assert::AreEqual(4, queue[1]);
			Assert::AreEqual(5, queue[2]);
		}

		TEST_METHOD(ZeroElement)
		{
			FixedSizeQueue<int> queue{ 0 };

			Assert::AreEqual(0, int(queue.size()));
			Assert::AreEqual(0, int(queue.maxSize()));

			queue.push(5);

			Assert::AreEqual(0, int(queue.size()));
			Assert::AreEqual(0, int(queue.maxSize()));
		}
	};
}