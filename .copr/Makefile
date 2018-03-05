srpm:
	dnf -y install @development-tools cmake radcli-devel nettle-devel
	mkdir -p build
	cd build && cmake .. && \
	  cpack -G RPM -D CPACK_RPM_PACKAGE_SOURCES=ON \
	  -D CPACK_OUTPUT_FILE_PREFIX=$(outdir)
