int main() {
	int i = 0;
	int a[3];
	int j = 3;

	while(i < j) {
		printf(i);
		a[i] = i;
		i = i + 1;
	}

	i = 0;
	while(i < j) {
		printf(a[i]);
		i = i + 1;
	}
	return 0;
}
