#pragma once
#include <vector>
#include <assert.h>
using namespace std;

template<typename E>
class Compare{
public:
	static bool prior(E& left, E& right){
		return left < right;
	}
};

template<typename E,typename Comp>
class MinHeap_self{
private:
	vector<E> Heap;
	int maxsize;
	int n;

	void siftdown(int pos){
		while (!isLeaf(pos)){
			int j = leftchild(pos);
			int rc = rightchild(pos);
			if ((rc < n) && Comp::prior(Heap[rc], Heap[j]))
				j = rc;
			if (Comp::prior(Heap[pos], Heap[j])) return;
			swap(pos, j);
			pos = j;
		}
	}

	void swap(int left, int right){
		if (left < 0 || right < 0 || left >= Heap.size() || right >= Heap.size()){
			cout << "Ô½½ç\n";
			return;
		}
		E temp = Heap[left];
		Heap[left] = Heap[right];
		Heap[right] = temp;
	}

public:
	MinHeap_self(){
		n = 0;
	}
	MinHeap_self(vector<E> h, int num, int max){
		Heap = h; n = num; maxsize = max; buildHeap();
	}
	int size() const { return n; }
	bool empty() { return n == 0; }
	bool isLeaf(int pos) const { return (pos >= n / 2) && (pos < n); }
	int leftchild(int pos) const{ return 2 * pos + 1; }
	int rightchild(int pos) const{ return 2 * pos + 2; }
	int parent(int pos) const{ return (pos - 1) / 2; }
	void buildHeap(){ for (int i = n / 2 - 1; i >= 0; i--) siftdown(i); }

	void insert(const E& it){
		//cout << "insert\n";
		int curr = n++;
		Heap.push_back(it);
		while ((curr != 0) &&
			(Comp::prior(Heap[curr], Heap[parent(curr)]))){
			swap(curr, parent(curr));
			curr = parent(curr);
		}
	}

	E removefirst(){
		//cout << "removefirst\n";
		assert(n > 0);
		E ret = Heap[0];
		swap(0, --n);

		if (n != 0) siftdown(0);
		Heap.pop_back();

		return ret;
	}

	E remove(int pos){
		//assert((pos >= 0) && (pos < n));

		E ret = Heap[pos];

		if (pos == n - 1) n--;
		else{
			swap(pos, --n);
			while ((pos != 0) &&
				(Comp::prior(Heap[pos], Heap[parent(pos)]))){
				swap(pos, parent(pos));
				pos = parent(pos);
			}
			if (n != 0) siftdown(pos);
		}
		Heap.pop_back();

		return ret;
	}

	void remove_value(E it){
		//cout << "remove_value\n";
		//cout << "n=" << n << " heap.size()=" << Heap.size() << endl;
		for (int i = 0; i < n; i++){
			if (Heap[i] == it)
			{
				remove(i);
				break;
			}
		}
	}

	bool isContains(E it){
		//cout << "isContains\n";
		for (int i = 0; i < n; i++){
			if (Heap[i] == it) return true;
		}
		return false;
	}
};