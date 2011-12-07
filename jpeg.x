
struct IMG {
	char* data;
};

program JPEG_SHRINK{
    version JPEG_VERS {
        struct IMG SHRINK_IMG(struct IMG) = 1;
    } = 1;
} = 0x31234567;  
