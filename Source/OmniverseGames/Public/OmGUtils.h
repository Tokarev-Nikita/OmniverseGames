#pragma once

template <typename T, typename Y>
void Shellsort(T& array, Y size) {
	register int gap, i, j, temp;
	gap = 1;
	do(gap = 3 * gap + 1); while (gap <= size);
	for (gap /= 3; gap > 0; gap /= 3)
		for (i = gap; i < size; ++i) {
			temp = array[i];
			for (j = i - gap; (j >= 0) && (array[j] > temp); j -= gap)
				array[j + gap] = array[j];
			array[j + gap] = temp;
		}
}