int main() {
	int vet[10];
	int i = 9;
	while(i > -1) {
		vet[i] = i+1;
		i = i - 1;
	}

	i = 0;
	int j = 0;
	int aux = 0;
	while(i < 9) {
		j = i+1;
		while(j < 10) {
			if(vet[i] > vet[j]) {
				aux = vet[i];
				vet[i] = vet[j];
				vet[j] = aux;
			}
			j = j + 1;
		}
		i = i + 1;
	}

	i = 0;
	while(i < 10) {
		printf(vet[i]);
		i = i + 1;
	}
	return 0;
}
