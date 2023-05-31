#include "../hdrs/PmergeMe.hpp"


void insertionSort(std::vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}
void merge(std::vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    std::vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++) {
        L[i] = arr[left + i];
    }
    for (int j = 0; j < n2; j++) {
        R[j] = arr[mid + 1 + j];
    }
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeInsertionSortVec(std::vector<int>& arr, int left, int right) {
    if (left < right) {
        if (right - left + 1 <= 10) {
            insertionSort(arr, left, right);
        }
        else
        {
            int mid = left + (right - left) / 2;
            mergeInsertionSortVec(arr, left, mid);
            mergeInsertionSortVec(arr, mid + 1, right);
            merge(arr, left, mid, right);
        }
    }
}

bool is_valid(std::string arg)
{
    for (size_t i = 0; i < arg.size(); i++)
    {
        if (!isdigit(arg[i]) && arg[i] != ' ')
        {
            std::cout << "Error : "<< arg[i];
            return false;
        }
    }
    return true;
}

void execute_vector(std::vector<int> vct)
{
    clock_t start = clock();
    mergeInsertionSortVec(vct, 0, vct.size() - 1);
    clock_t end = clock();
    std::cout << "After  :   ";
    for (size_t i = 0; i < vct.size(); i++) {
        std::cout << vct[i] << " ";
    }
    std::cout << std::endl;
    double duration = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "Time to process a range of "<< vct.size() << " elements with std::vector " << duration * 1000 << " ms" << std::endl;
}

void insertionSort(std::list<int>::iterator left, std::list<int>::iterator right) {
    for (std::list<int>::iterator it = left; it != right; ++it) {
        int key = *it;
        std::list<int>::iterator j = it;
        while (j != left && *j < *--j) {
            std::swap(*j, *(++j));
            --j;
        }
        *j = key;
    }
}

void merge(std::list<int>::iterator left, std::list<int>::iterator mid, std::list<int>::iterator right) {
    std::list<int> merged;
    std::list<int>::iterator i = left;
    std::list<int>::iterator j = mid;
    while (i != mid && j != right) {
        if (*i <= *j) {
            merged.push_back(*i);
            ++i;
        }
        else {
            merged.push_back(*j);
            ++j;
        }
    }
    merged.insert(merged.end(), i, mid);
    merged.insert(merged.end(), j, right);
    std::copy(merged.begin(), merged.end(), left);
}

void mergeInsertionSortList(std::list<int>::iterator left, std::list<int>::iterator right) {
    if (std::distance(left, right) > 10) {
        std::list<int>::iterator mid = left;
        std::advance(mid, std::distance(left, right) / 2);
        mergeInsertionSortList(left, mid);
        mergeInsertionSortList(mid, right);
        merge(left, mid, right);
    }
    else {
        insertionSort(left, right);
    }
}

void execute_list(std::list<int> lst)
{
    clock_t start = clock();
    mergeInsertionSortList(lst.begin(), lst.end());
    clock_t end = clock();
    double duration = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    std::cout << "Time to process a range of " << lst.size() << " elements with std::list: " << duration * 1000 << " ms" << std::endl;
}