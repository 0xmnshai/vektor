class VPI_ISystem {
protected:
	VPI_ISystem() = default;
	virtual ~VPI_ISystem() = default;
	virtual uint64_t get_milliseconds() const noexcept = 0;
};
