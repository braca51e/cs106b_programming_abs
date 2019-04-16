#include "HeapPQueue.h"
#include "Testing/HeapTests.h"
using namespace std;

HeapPQueue::HeapPQueue() {
    /* TODO: Implement this. */
}

HeapPQueue::~HeapPQueue() {
    /* TODO: Implement this. */
}

void HeapPQueue::enqueue(const DataPoint& data) {
    /* TODO: Delete the next line and implement this. */
    (void) data;
}

int HeapPQueue::size() const {
    /* TODO: Delete the next line and implement this. */
    return 0;
}

DataPoint HeapPQueue::peek() const {
    /* TODO: Delete the next line and implement this. */
    return {};
}

DataPoint HeapPQueue::dequeue() {
    /* TODO: Delete the next line and implement this. */
    return {};
}

bool HeapPQueue::isEmpty() const {
    /* TODO: Delete the next line and implement this. */
    return 0;
}

/* This function is purely for you to use during testing. You can have it do whatever
 * you'd like, including nothing. We won't call this function during grading, so feel
 * free to fill it with whatever is most useful to you!
 *
 * TODO: Delete this comment and replace it with one describing what this function
 * actually does.
 */
void HeapPQueue::printDebugInfo() {
    /* TODO: Delete this comment and (optionally) put debugging code here. */
}




/* * * * * * Test Cases Below This Point * * * * * */

ADD_TEST("Newly-created heap is empty.") {
    HeapPQueue pq;

    EXPECT(pq.isEmpty());
    EXPECT(pq.size() == 0);
    EXPECT_ERROR(pq.dequeue());
}
