struct file_list {
	char *a_filename;
	char *a_dir;
	struct idf *a_idf;
	struct file_list *a_next;
};
