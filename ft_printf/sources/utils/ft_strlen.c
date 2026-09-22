unsigned long long ft_strlen(char *s) {
	unsigned long long size;

	size = 0;
	if (!s)
		return (0);
	while (s[size])
		size++;
	return (size);
}
