begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|DELTA_H
end_ifndef
begin_define
DECL|macro|DELTA_H
define|#
directive|define
name|DELTA_H
end_define
begin_comment
comment|/* opaque object for delta index */
end_comment
begin_struct_decl
struct_decl|struct
name|delta_index
struct_decl|;
end_struct_decl
begin_comment
comment|/*  * create_delta_index: compute index data from given buffer  *  * This returns a pointer to a struct delta_index that should be passed to  * subsequent create_delta() calls, or to free_delta_index().  A NULL pointer  * is returned on failure.  The given buffer must not be freed or altered  * before free_delta_index() is called.  The returned pointer must be freed  * using free_delta_index().  */
end_comment
begin_function_decl
specifier|extern
name|struct
name|delta_index
modifier|*
name|create_delta_index
parameter_list|(
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|bufsize
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * free_delta_index: free the index created by create_delta_index()  *  * Given pointer must be what create_delta_index() returned, or NULL.  */
end_comment
begin_function_decl
specifier|extern
name|void
name|free_delta_index
parameter_list|(
name|struct
name|delta_index
modifier|*
name|index
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * sizeof_delta_index: returns memory usage of delta index  *  * Given pointer must be what create_delta_index() returned, or NULL.  */
end_comment
begin_function_decl
specifier|extern
name|unsigned
name|long
name|sizeof_delta_index
parameter_list|(
name|struct
name|delta_index
modifier|*
name|index
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * create_delta: create a delta from given index for the given buffer  *  * This function may be called multiple times with different buffers using  * the same delta_index pointer.  If max_delta_size is non-zero and the  * resulting delta is to be larger than max_delta_size then NULL is returned.  * On success, a non-NULL pointer to the buffer with the delta data is  * returned and *delta_size is updated with its size.  The returned buffer  * must be freed by the caller.  */
end_comment
begin_function_decl
specifier|extern
name|void
modifier|*
name|create_delta
parameter_list|(
specifier|const
name|struct
name|delta_index
modifier|*
name|index
parameter_list|,
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|bufsize
parameter_list|,
name|unsigned
name|long
modifier|*
name|delta_size
parameter_list|,
name|unsigned
name|long
name|max_delta_size
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * diff_delta: create a delta from source buffer to target buffer  *  * If max_delta_size is non-zero and the resulting delta is to be larger  * than max_delta_size then NULL is returned.  On success, a non-NULL  * pointer to the buffer with the delta data is returned and *delta_size is  * updated with its size.  The returned buffer must be freed by the caller.  */
end_comment
begin_function
specifier|static
specifier|inline
name|void
modifier|*
DECL|function|diff_delta
name|diff_delta
parameter_list|(
specifier|const
name|void
modifier|*
name|src_buf
parameter_list|,
name|unsigned
name|long
name|src_bufsize
parameter_list|,
specifier|const
name|void
modifier|*
name|trg_buf
parameter_list|,
name|unsigned
name|long
name|trg_bufsize
parameter_list|,
name|unsigned
name|long
modifier|*
name|delta_size
parameter_list|,
name|unsigned
name|long
name|max_delta_size
parameter_list|)
block|{
name|struct
name|delta_index
modifier|*
name|index
init|=
name|create_delta_index
argument_list|(
name|src_buf
argument_list|,
name|src_bufsize
argument_list|)
decl_stmt|;
if|if
condition|(
name|index
condition|)
block|{
name|void
modifier|*
name|delta
init|=
name|create_delta
argument_list|(
name|index
argument_list|,
name|trg_buf
argument_list|,
name|trg_bufsize
argument_list|,
name|delta_size
argument_list|,
name|max_delta_size
argument_list|)
decl_stmt|;
name|free_delta_index
argument_list|(
name|index
argument_list|)
expr_stmt|;
return|return
name|delta
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_comment
comment|/*  * patch_delta: recreate target buffer given source buffer and delta data  *  * On success, a non-NULL pointer to the target buffer is returned and  * *trg_bufsize is updated with its size.  On failure a NULL pointer is  * returned.  The returned buffer must be freed by the caller.  */
end_comment
begin_function_decl
specifier|extern
name|void
modifier|*
name|patch_delta
parameter_list|(
specifier|const
name|void
modifier|*
name|src_buf
parameter_list|,
name|unsigned
name|long
name|src_size
parameter_list|,
specifier|const
name|void
modifier|*
name|delta_buf
parameter_list|,
name|unsigned
name|long
name|delta_size
parameter_list|,
name|unsigned
name|long
modifier|*
name|dst_size
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* the smallest possible delta size is 4 bytes */
end_comment
begin_define
DECL|macro|DELTA_SIZE_MIN
define|#
directive|define
name|DELTA_SIZE_MIN
value|4
end_define
begin_comment
comment|/*  * This must be called twice on the delta data buffer, first to get the  * expected source buffer size, and again to get the target buffer size.  */
end_comment
begin_function
DECL|function|get_delta_hdr_size
specifier|static
specifier|inline
name|unsigned
name|long
name|get_delta_hdr_size
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
modifier|*
name|datap
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|top
parameter_list|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|data
init|=
operator|*
name|datap
decl_stmt|;
name|unsigned
name|long
name|cmd
decl_stmt|,
name|size
init|=
literal|0
decl_stmt|;
name|int
name|i
init|=
literal|0
decl_stmt|;
do|do
block|{
name|cmd
operator|=
operator|*
name|data
operator|++
expr_stmt|;
name|size
operator||=
operator|(
name|cmd
operator|&
literal|0x7f
operator|)
operator|<<
name|i
expr_stmt|;
name|i
operator|+=
literal|7
expr_stmt|;
block|}
do|while
condition|(
name|cmd
operator|&
literal|0x80
operator|&&
name|data
operator|<
name|top
condition|)
do|;
operator|*
name|datap
operator|=
name|data
expr_stmt|;
return|return
name|size
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
end_unit
