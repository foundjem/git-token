begin_unit
begin_comment
comment|/*  * Converts filenames from decomposed unicode into precomposed unicode.  * Used on MacOS X.  */
end_comment
begin_define
DECL|macro|PRECOMPOSE_UNICODE_C
define|#
directive|define
name|PRECOMPOSE_UNICODE_C
end_define
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"utf8.h"
end_include
begin_include
include|#
directive|include
file|"precompose_utf8.h"
end_include
begin_typedef
DECL|typedef|iconv_ibp
typedef|typedef
name|char
modifier|*
name|iconv_ibp
typedef|;
end_typedef
begin_decl_stmt
DECL|variable|repo_encoding
specifier|static
specifier|const
name|char
modifier|*
name|repo_encoding
init|=
literal|"UTF-8"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|path_encoding
specifier|static
specifier|const
name|char
modifier|*
name|path_encoding
init|=
literal|"UTF-8-MAC"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|has_non_ascii
specifier|static
name|size_t
name|has_non_ascii
parameter_list|(
specifier|const
name|char
modifier|*
name|s
parameter_list|,
name|size_t
name|maxlen
parameter_list|,
name|size_t
modifier|*
name|strlen_c
parameter_list|)
block|{
specifier|const
name|uint8_t
modifier|*
name|ptr
init|=
operator|(
specifier|const
name|uint8_t
operator|*
operator|)
name|s
decl_stmt|;
name|size_t
name|strlen_chars
init|=
literal|0
decl_stmt|;
name|size_t
name|ret
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|!
name|ptr
operator|||
operator|!
operator|*
name|ptr
condition|)
return|return
literal|0
return|;
while|while
condition|(
operator|*
name|ptr
operator|&&
name|maxlen
condition|)
block|{
if|if
condition|(
operator|*
name|ptr
operator|&
literal|0x80
condition|)
name|ret
operator|++
expr_stmt|;
name|strlen_chars
operator|++
expr_stmt|;
name|ptr
operator|++
expr_stmt|;
name|maxlen
operator|--
expr_stmt|;
block|}
if|if
condition|(
name|strlen_c
condition|)
operator|*
name|strlen_c
operator|=
name|strlen_chars
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|probe_utf8_pathname_composition
name|void
name|probe_utf8_pathname_composition
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|strbuf
name|path
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|static
specifier|const
name|char
modifier|*
name|auml_nfc
init|=
literal|"\xc3\xa4"
decl_stmt|;
specifier|static
specifier|const
name|char
modifier|*
name|auml_nfd
init|=
literal|"\x61\xcc\x88"
decl_stmt|;
name|int
name|output_fd
decl_stmt|;
if|if
condition|(
name|precomposed_unicode
operator|!=
operator|-
literal|1
condition|)
return|return;
comment|/* We found it defined in the global config, respect it */
name|git_path_buf
argument_list|(
operator|&
name|path
argument_list|,
literal|"%s"
argument_list|,
name|auml_nfc
argument_list|)
expr_stmt|;
name|output_fd
operator|=
name|open
argument_list|(
name|path
operator|.
name|buf
argument_list|,
name|O_CREAT
operator||
name|O_EXCL
operator||
name|O_RDWR
argument_list|,
literal|0600
argument_list|)
expr_stmt|;
if|if
condition|(
name|output_fd
operator|>=
literal|0
condition|)
block|{
name|close
argument_list|(
name|output_fd
argument_list|)
expr_stmt|;
name|git_path_buf
argument_list|(
operator|&
name|path
argument_list|,
literal|"%s"
argument_list|,
name|auml_nfd
argument_list|)
expr_stmt|;
name|precomposed_unicode
operator|=
name|access
argument_list|(
name|path
operator|.
name|buf
argument_list|,
name|R_OK
argument_list|)
condition|?
literal|0
else|:
literal|1
expr_stmt|;
name|git_config_set
argument_list|(
literal|"core.precomposeunicode"
argument_list|,
name|precomposed_unicode
condition|?
literal|"true"
else|:
literal|"false"
argument_list|)
expr_stmt|;
name|git_path_buf
argument_list|(
operator|&
name|path
argument_list|,
literal|"%s"
argument_list|,
name|auml_nfc
argument_list|)
expr_stmt|;
if|if
condition|(
name|unlink
argument_list|(
name|path
operator|.
name|buf
argument_list|)
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"failed to unlink '%s'"
argument_list|)
argument_list|,
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|precompose_argv
name|void
name|precompose_argv
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|i
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|oldarg
decl_stmt|;
name|char
modifier|*
name|newarg
decl_stmt|;
name|iconv_t
name|ic_precompose
decl_stmt|;
if|if
condition|(
name|precomposed_unicode
operator|!=
literal|1
condition|)
return|return;
name|ic_precompose
operator|=
name|iconv_open
argument_list|(
name|repo_encoding
argument_list|,
name|path_encoding
argument_list|)
expr_stmt|;
if|if
condition|(
name|ic_precompose
operator|==
operator|(
name|iconv_t
operator|)
operator|-
literal|1
condition|)
return|return;
while|while
condition|(
name|i
operator|<
name|argc
condition|)
block|{
name|size_t
name|namelen
decl_stmt|;
name|oldarg
operator|=
name|argv
index|[
name|i
index|]
expr_stmt|;
if|if
condition|(
name|has_non_ascii
argument_list|(
name|oldarg
argument_list|,
operator|(
name|size_t
operator|)
operator|-
literal|1
argument_list|,
operator|&
name|namelen
argument_list|)
condition|)
block|{
name|newarg
operator|=
name|reencode_string_iconv
argument_list|(
name|oldarg
argument_list|,
name|namelen
argument_list|,
name|ic_precompose
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|newarg
condition|)
name|argv
index|[
name|i
index|]
operator|=
name|newarg
expr_stmt|;
block|}
name|i
operator|++
expr_stmt|;
block|}
name|iconv_close
argument_list|(
name|ic_precompose
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|precompose_utf8_opendir
name|PREC_DIR
modifier|*
name|precompose_utf8_opendir
parameter_list|(
specifier|const
name|char
modifier|*
name|dirname
parameter_list|)
block|{
name|PREC_DIR
modifier|*
name|prec_dir
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|PREC_DIR
argument_list|)
argument_list|)
decl_stmt|;
name|prec_dir
operator|->
name|dirent_nfc
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|dirent_prec_psx
argument_list|)
argument_list|)
expr_stmt|;
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|max_name_len
operator|=
sizeof|sizeof
argument_list|(
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|d_name
argument_list|)
expr_stmt|;
name|prec_dir
operator|->
name|dirp
operator|=
name|opendir
argument_list|(
name|dirname
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|prec_dir
operator|->
name|dirp
condition|)
block|{
name|free
argument_list|(
name|prec_dir
operator|->
name|dirent_nfc
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|prec_dir
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
else|else
block|{
name|int
name|ret_errno
init|=
name|errno
decl_stmt|;
name|prec_dir
operator|->
name|ic_precompose
operator|=
name|iconv_open
argument_list|(
name|repo_encoding
argument_list|,
name|path_encoding
argument_list|)
expr_stmt|;
comment|/* if iconv_open() fails, die() in readdir() if needed */
name|errno
operator|=
name|ret_errno
expr_stmt|;
block|}
return|return
name|prec_dir
return|;
block|}
end_function
begin_function
DECL|function|precompose_utf8_readdir
name|struct
name|dirent_prec_psx
modifier|*
name|precompose_utf8_readdir
parameter_list|(
name|PREC_DIR
modifier|*
name|prec_dir
parameter_list|)
block|{
name|struct
name|dirent
modifier|*
name|res
decl_stmt|;
name|res
operator|=
name|readdir
argument_list|(
name|prec_dir
operator|->
name|dirp
argument_list|)
expr_stmt|;
if|if
condition|(
name|res
condition|)
block|{
name|size_t
name|namelenz
init|=
name|strlen
argument_list|(
name|res
operator|->
name|d_name
argument_list|)
operator|+
literal|1
decl_stmt|;
comment|/* \0 */
name|size_t
name|new_maxlen
init|=
name|namelenz
decl_stmt|;
name|int
name|ret_errno
init|=
name|errno
decl_stmt|;
if|if
condition|(
name|new_maxlen
operator|>
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|max_name_len
condition|)
block|{
name|size_t
name|new_len
init|=
sizeof|sizeof
argument_list|(
name|dirent_prec_psx
argument_list|)
operator|+
name|new_maxlen
operator|-
sizeof|sizeof
argument_list|(
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|d_name
argument_list|)
decl_stmt|;
name|prec_dir
operator|->
name|dirent_nfc
operator|=
name|xrealloc
argument_list|(
name|prec_dir
operator|->
name|dirent_nfc
argument_list|,
name|new_len
argument_list|)
expr_stmt|;
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|max_name_len
operator|=
name|new_maxlen
expr_stmt|;
block|}
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|d_ino
operator|=
name|res
operator|->
name|d_ino
expr_stmt|;
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|d_type
operator|=
name|res
operator|->
name|d_type
expr_stmt|;
if|if
condition|(
operator|(
name|precomposed_unicode
operator|==
literal|1
operator|)
operator|&&
name|has_non_ascii
argument_list|(
name|res
operator|->
name|d_name
argument_list|,
operator|(
name|size_t
operator|)
operator|-
literal|1
argument_list|,
name|NULL
argument_list|)
condition|)
block|{
if|if
condition|(
name|prec_dir
operator|->
name|ic_precompose
operator|==
operator|(
name|iconv_t
operator|)
operator|-
literal|1
condition|)
block|{
name|die
argument_list|(
literal|"iconv_open(%s,%s) failed, but needed:\n"
literal|"    precomposed unicode is not supported.\n"
literal|"    If you want to use decomposed unicode, run\n"
literal|"    \"git config core.precomposeunicode false\"\n"
argument_list|,
name|repo_encoding
argument_list|,
name|path_encoding
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|iconv_ibp
name|cp
init|=
operator|(
name|iconv_ibp
operator|)
name|res
operator|->
name|d_name
decl_stmt|;
name|size_t
name|inleft
init|=
name|namelenz
decl_stmt|;
name|char
modifier|*
name|outpos
init|=
operator|&
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|d_name
index|[
literal|0
index|]
decl_stmt|;
name|size_t
name|outsz
init|=
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|max_name_len
decl_stmt|;
name|errno
operator|=
literal|0
expr_stmt|;
name|iconv
argument_list|(
name|prec_dir
operator|->
name|ic_precompose
argument_list|,
operator|&
name|cp
argument_list|,
operator|&
name|inleft
argument_list|,
operator|&
name|outpos
argument_list|,
operator|&
name|outsz
argument_list|)
expr_stmt|;
if|if
condition|(
name|errno
operator|||
name|inleft
condition|)
block|{
comment|/* 					 * iconv() failed and errno could be E2BIG, EILSEQ, EINVAL, EBADF 					 * MacOS X avoids illegal byte sequences. 					 * If they occur on a mounted drive (e.g. NFS) it is not worth to 					 * die() for that, but rather let the user see the original name 					*/
name|namelenz
operator|=
literal|0
expr_stmt|;
comment|/* trigger strlcpy */
block|}
block|}
block|}
else|else
name|namelenz
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|namelenz
condition|)
name|strlcpy
argument_list|(
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|d_name
argument_list|,
name|res
operator|->
name|d_name
argument_list|,
name|prec_dir
operator|->
name|dirent_nfc
operator|->
name|max_name_len
argument_list|)
expr_stmt|;
name|errno
operator|=
name|ret_errno
expr_stmt|;
return|return
name|prec_dir
operator|->
name|dirent_nfc
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|precompose_utf8_closedir
name|int
name|precompose_utf8_closedir
parameter_list|(
name|PREC_DIR
modifier|*
name|prec_dir
parameter_list|)
block|{
name|int
name|ret_value
decl_stmt|;
name|int
name|ret_errno
decl_stmt|;
name|ret_value
operator|=
name|closedir
argument_list|(
name|prec_dir
operator|->
name|dirp
argument_list|)
expr_stmt|;
name|ret_errno
operator|=
name|errno
expr_stmt|;
if|if
condition|(
name|prec_dir
operator|->
name|ic_precompose
operator|!=
operator|(
name|iconv_t
operator|)
operator|-
literal|1
condition|)
name|iconv_close
argument_list|(
name|prec_dir
operator|->
name|ic_precompose
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|prec_dir
operator|->
name|dirent_nfc
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|prec_dir
argument_list|)
expr_stmt|;
name|errno
operator|=
name|ret_errno
expr_stmt|;
return|return
name|ret_value
return|;
block|}
end_function
end_unit
