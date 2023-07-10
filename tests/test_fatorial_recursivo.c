int fatorial(int n) {
	if (n < 2) {
		return 1;
	}
	return n * fatorial(n-1);
}

int main() {
	int fat = fatorial(5);
	printf(fat);
	return 0;
}
