/**
 * Implementation of arbitrary length LFSR.
 * Input: binary sequence describing the LFSR.
 * Output: binary sequence generated by the LFSR in one period.
 */

#include "lfsr.h"

/**
 * Circularly shift the given char array (strictly null-terminated) right.
 */
void circular_shift_right(int arr[], int l) {
	int i = 0;
	int last = arr[l - 1];
	for (i = l - 1; i >= 1; i--) {
		arr[i] = arr[i - 1];
	}
	arr[i] = last;
}

// TOFIX!
void get_str_bits(char str[], int bits[]) {
	int i = 0;
	for (i = 0; i < strlen(str) * 8; i++) {
		bits[i] = (str[i / 8] << i) & 0x80; // Take the i-th MSB
	}
}

/**
 * Run the given LFSR howmany times and store the result as a
 * string into the given char array.
 */
void get_lfsr_output(LFSR *lfsr_in, int lfsr_output[], int howmany) {
	int *lfsr_descriptor = lfsr_in->descriptor,
		*lfsr_state = lfsr_in->initial_state,
		lfsr_len = lfsr_in->len;
	unsigned int j;
	int i = 0,
		is_first_digit,
		cur_desc_digit, 
		cur_state_digit,
		best_period = pow(2, lfsr_len) - 1,
		period = (howmany > 0)? howmany : best_period,
		new_digit;

	// printf("Best expected period: %d\n", best_period);
	j = 0;
	while (j < period) {
		for (i = 0, is_first_digit = 1; i < lfsr_len; i++) {
			cur_desc_digit = lfsr_descriptor[i];
			// printf("Pos %d is %d\n", i, cur_desc_digit);
			if (cur_desc_digit == 1) {
				cur_state_digit = lfsr_state[i];
				if (is_first_digit) {
					new_digit = cur_state_digit;
					is_first_digit = 0;
					// printf("First digit! Result is %d, value is %d\n", result_digit, cur_state_digit);	
				} else {
					new_digit ^= cur_state_digit;					
					// printf("...Value is %d, result is %d\n", cur_state_digit, result_digit);	
				}
			}
			// printf("Processing (%s) (%d) %d\n", lfsr_state, i, cur_state_digit);
		}
		lfsr_output[j] = lfsr_state[i - 1];
		circular_shift_right(lfsr_state, lfsr_len);
		lfsr_state[0] = new_digit;
		j++;
	}
}

char c2b(char c) {
	return (c == '0')? 0 : 1;
}

void get_poly_str(int poly[], int n, char buffer[]) {
	int i = 0;
	char final[1024] = {0};
	sprintf(final, "%s%d", final, poly[0]);
	for (i = 1; i < n; ++i) // From 1 because first coefficient is already included
	{
		if (poly[i] == 1) {
			sprintf(final, "%s + ", final);
			sprintf(final, "%sx^%d", final, i);
		}
	}
	sprintf(final, "%s%c", final, '\0');
	// printf("%s\n", final);
	strcpy(buffer, final);
}

/**
 * Compute Berlekamp-Massey Algorithm for the given binary sequence.
 * Return the linear complexity and store the LFSR descriptor in the
 * given array as a string.
 */
int bma(int sequence[], int desc[], int n) {
	int size_n = n * sizeof(int); // You never know!
	int C[n], B[n], T[n];
	int L, m, i, delta; // Main variables of the algorithm
	int j, k, B_shift; // Auxiliary variables
	// char C_str[1024], B_str[1024];

	memset(C, 0, size_n);
	memset(B, 0, size_n);
	memset(T, 0, size_n);

	C[0] = B[0] = 1;
	L = 0;
	m = -1;
	i = 0;

	while (i < n) {
		// get_poly_str(C, n, C_str);
		// get_poly_str(B, n, B_str);
		// Compute delta as (s(i) + c1*s(i-1) + ... + cL*s(i-L))
		// printf("Delta init to s%d\n", i);
		delta = sequence[i];
		// printf("First value of delta: %d\n", delta);
		for (j = 1; j <= L; ++j)
		{
			delta  ^= sequence[i - j] * C[j];
			// printf("Adding s%d * C[%d]\n", i-j, j - 1);
		}
		// printf("Step %d:\n\tC(x) = %s\n\tL = %d, m = %d\n\tB(x) = %s\n\ti = %d\n\tdelta = %d\n", i, C_str, L, m, B_str, i, delta);
		if (delta == 1) {
			memcpy(T, C, size_n);
			B_shift = i - m;
			// We have to xor each element of C with each element of B shifted by B_shift positions right
			for (k = 0; k < n - B_shift; k++) {
				// We need to start xoring from the B_shiftth element
				C[k + B_shift] ^= B[k]; 
			}
			if (L <= i/2) {
				L = i + 1 - L;
				m = i;
				memcpy(B, T, size_n);
			}
		}
		i++;
	}

	// We want a string at the end
	memcpy(desc, C, size_n);
	return L;
}

void print_array(int arr[], int l) {
	int i = 0;
	for (i = 0; i < l; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
} 
