
struct img_t {
  char data<>;
	int suc;
};

program JPEG_SHRINK{
    version JPEG_VERS {
        img_t SHRINK_IMG(img_t) = 1;
    } = 1;
} = 0x31234567;  
