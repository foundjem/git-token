begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"attr.h"
end_include
begin_include
include|#
directive|include
file|"archive.h"
end_include
begin_function
DECL|function|format_subst
specifier|static
name|void
name|format_subst
parameter_list|(
specifier|const
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|size_t
name|len
parameter_list|,
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|)
block|{
name|char
modifier|*
name|to_free
init|=
name|NULL
decl_stmt|;
name|struct
name|strbuf
name|fmt
decl_stmt|;
if|if
condition|(
name|src
operator|==
name|buf
operator|->
name|buf
condition|)
name|to_free
operator|=
name|strbuf_detach
argument_list|(
name|buf
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|fmt
argument_list|,
literal|0
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
specifier|const
name|char
modifier|*
name|b
decl_stmt|,
modifier|*
name|c
decl_stmt|;
name|b
operator|=
name|memmem
argument_list|(
name|src
argument_list|,
name|len
argument_list|,
literal|"$Format:"
argument_list|,
literal|8
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|b
condition|)
break|break;
name|c
operator|=
name|memchr
argument_list|(
name|b
operator|+
literal|8
argument_list|,
literal|'$'
argument_list|,
operator|(
name|src
operator|+
name|len
operator|)
operator|-
name|b
operator|-
literal|8
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|c
condition|)
break|break;
name|strbuf_reset
argument_list|(
operator|&
name|fmt
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
operator|&
name|fmt
argument_list|,
name|b
operator|+
literal|8
argument_list|,
name|c
operator|-
name|b
operator|-
literal|8
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|buf
argument_list|,
name|src
argument_list|,
name|b
operator|-
name|src
argument_list|)
expr_stmt|;
name|format_commit_message
argument_list|(
name|commit
argument_list|,
name|fmt
operator|.
name|buf
argument_list|,
name|buf
argument_list|)
expr_stmt|;
name|len
operator|-=
name|c
operator|+
literal|1
operator|-
name|src
expr_stmt|;
name|src
operator|=
name|c
operator|+
literal|1
expr_stmt|;
block|}
name|strbuf_add
argument_list|(
name|buf
argument_list|,
name|src
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|fmt
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|to_free
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|sha1_file_to_archive
specifier|static
name|void
modifier|*
name|sha1_file_to_archive
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|unsigned
name|int
name|mode
parameter_list|,
name|enum
name|object_type
modifier|*
name|type
parameter_list|,
name|unsigned
name|long
modifier|*
name|sizep
parameter_list|,
specifier|const
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|void
modifier|*
name|buffer
decl_stmt|;
name|buffer
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
name|sizep
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer
operator|&&
name|S_ISREG
argument_list|(
name|mode
argument_list|)
condition|)
block|{
name|struct
name|strbuf
name|buf
decl_stmt|;
name|size_t
name|size
init|=
literal|0
decl_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|buf
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_attach
argument_list|(
operator|&
name|buf
argument_list|,
name|buffer
argument_list|,
operator|*
name|sizep
argument_list|,
operator|*
name|sizep
operator|+
literal|1
argument_list|)
expr_stmt|;
name|convert_to_working_tree
argument_list|(
name|path
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|buf
operator|.
name|len
argument_list|,
operator|&
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
condition|)
name|format_subst
argument_list|(
name|commit
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|buf
operator|.
name|len
argument_list|,
operator|&
name|buf
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|buf
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
operator|*
name|sizep
operator|=
name|size
expr_stmt|;
block|}
return|return
name|buffer
return|;
block|}
end_function
begin_function
DECL|function|setup_archive_check
specifier|static
name|void
name|setup_archive_check
parameter_list|(
name|struct
name|git_attr_check
modifier|*
name|check
parameter_list|)
block|{
specifier|static
name|struct
name|git_attr
modifier|*
name|attr_export_ignore
decl_stmt|;
specifier|static
name|struct
name|git_attr
modifier|*
name|attr_export_subst
decl_stmt|;
if|if
condition|(
operator|!
name|attr_export_ignore
condition|)
block|{
name|attr_export_ignore
operator|=
name|git_attr
argument_list|(
literal|"export-ignore"
argument_list|,
literal|13
argument_list|)
expr_stmt|;
name|attr_export_subst
operator|=
name|git_attr
argument_list|(
literal|"export-subst"
argument_list|,
literal|12
argument_list|)
expr_stmt|;
block|}
name|check
index|[
literal|0
index|]
operator|.
name|attr
operator|=
name|attr_export_ignore
expr_stmt|;
name|check
index|[
literal|1
index|]
operator|.
name|attr
operator|=
name|attr_export_subst
expr_stmt|;
block|}
end_function
begin_struct
DECL|struct|archiver_context
struct|struct
name|archiver_context
block|{
DECL|member|args
name|struct
name|archiver_args
modifier|*
name|args
decl_stmt|;
DECL|member|write_entry
name|write_archive_entry_fn_t
name|write_entry
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|write_archive_entry
specifier|static
name|int
name|write_archive_entry
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
specifier|const
name|char
modifier|*
name|filename
parameter_list|,
name|unsigned
name|mode
parameter_list|,
name|int
name|stage
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
block|{
specifier|static
name|struct
name|strbuf
name|path
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|archiver_context
modifier|*
name|c
init|=
name|context
decl_stmt|;
name|struct
name|archiver_args
modifier|*
name|args
init|=
name|c
operator|->
name|args
decl_stmt|;
name|write_archive_entry_fn_t
name|write_entry
init|=
name|c
operator|->
name|write_entry
decl_stmt|;
name|struct
name|git_attr_check
name|check
index|[
literal|2
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|path_without_prefix
decl_stmt|;
name|int
name|convert
init|=
literal|0
decl_stmt|;
name|int
name|err
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|void
modifier|*
name|buffer
decl_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|path
argument_list|)
expr_stmt|;
name|strbuf_grow
argument_list|(
operator|&
name|path
argument_list|,
name|PATH_MAX
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
operator|&
name|path
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|path
argument_list|,
name|filename
argument_list|)
expr_stmt|;
name|path_without_prefix
operator|=
name|path
operator|.
name|buf
operator|+
name|args
operator|->
name|baselen
expr_stmt|;
name|setup_archive_check
argument_list|(
name|check
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_checkattr
argument_list|(
name|path_without_prefix
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|check
argument_list|)
argument_list|,
name|check
argument_list|)
condition|)
block|{
if|if
condition|(
name|ATTR_TRUE
argument_list|(
name|check
index|[
literal|0
index|]
operator|.
name|value
argument_list|)
condition|)
return|return
literal|0
return|;
name|convert
operator|=
name|ATTR_TRUE
argument_list|(
name|check
index|[
literal|1
index|]
operator|.
name|value
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
operator|||
name|S_ISGITLINK
argument_list|(
name|mode
argument_list|)
condition|)
block|{
name|strbuf_addch
argument_list|(
operator|&
name|path
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
name|args
operator|->
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%.*s\n"
argument_list|,
operator|(
name|int
operator|)
name|path
operator|.
name|len
argument_list|,
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
name|err
operator|=
name|write_entry
argument_list|(
name|args
argument_list|,
name|sha1
argument_list|,
name|path
operator|.
name|buf
argument_list|,
name|path
operator|.
name|len
argument_list|,
name|mode
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
condition|)
return|return
name|err
return|;
return|return
name|READ_TREE_RECURSIVE
return|;
block|}
name|buffer
operator|=
name|sha1_file_to_archive
argument_list|(
name|path_without_prefix
argument_list|,
name|sha1
argument_list|,
name|mode
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|size
argument_list|,
name|convert
condition|?
name|args
operator|->
name|commit
else|:
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buffer
condition|)
return|return
name|error
argument_list|(
literal|"cannot read %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|args
operator|->
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%.*s\n"
argument_list|,
operator|(
name|int
operator|)
name|path
operator|.
name|len
argument_list|,
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
name|err
operator|=
name|write_entry
argument_list|(
name|args
argument_list|,
name|sha1
argument_list|,
name|path
operator|.
name|buf
argument_list|,
name|path
operator|.
name|len
argument_list|,
name|mode
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|err
return|;
block|}
end_function
begin_function
DECL|function|write_archive_entries
name|int
name|write_archive_entries
parameter_list|(
name|struct
name|archiver_args
modifier|*
name|args
parameter_list|,
name|write_archive_entry_fn_t
name|write_entry
parameter_list|)
block|{
name|struct
name|archiver_context
name|context
decl_stmt|;
name|int
name|err
decl_stmt|;
if|if
condition|(
name|args
operator|->
name|baselen
operator|>
literal|0
operator|&&
name|args
operator|->
name|base
index|[
name|args
operator|->
name|baselen
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
block|{
name|size_t
name|len
init|=
name|args
operator|->
name|baselen
decl_stmt|;
while|while
condition|(
name|len
operator|>
literal|1
operator|&&
name|args
operator|->
name|base
index|[
name|len
operator|-
literal|2
index|]
operator|==
literal|'/'
condition|)
name|len
operator|--
expr_stmt|;
if|if
condition|(
name|args
operator|->
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%.*s\n"
argument_list|,
operator|(
name|int
operator|)
name|len
argument_list|,
name|args
operator|->
name|base
argument_list|)
expr_stmt|;
name|err
operator|=
name|write_entry
argument_list|(
name|args
argument_list|,
name|args
operator|->
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|args
operator|->
name|base
argument_list|,
name|len
argument_list|,
literal|040777
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
condition|)
return|return
name|err
return|;
block|}
name|context
operator|.
name|args
operator|=
name|args
expr_stmt|;
name|context
operator|.
name|write_entry
operator|=
name|write_entry
expr_stmt|;
name|err
operator|=
name|read_tree_recursive
argument_list|(
name|args
operator|->
name|tree
argument_list|,
name|args
operator|->
name|base
argument_list|,
name|args
operator|->
name|baselen
argument_list|,
literal|0
argument_list|,
name|args
operator|->
name|pathspec
argument_list|,
name|write_archive_entry
argument_list|,
operator|&
name|context
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
operator|==
name|READ_TREE_RECURSIVE
condition|)
name|err
operator|=
literal|0
expr_stmt|;
return|return
name|err
return|;
block|}
end_function
begin_function
DECL|function|write_archive
name|int
name|write_archive
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|int
name|setup_prefix
parameter_list|)
block|{
specifier|const
name|struct
name|archiver
modifier|*
name|ar
init|=
name|NULL
decl_stmt|;
name|struct
name|archiver_args
name|args
decl_stmt|;
name|int
name|tree_idx
decl_stmt|;
name|tree_idx
operator|=
name|parse_archive_args
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|ar
argument_list|,
operator|&
name|args
argument_list|)
expr_stmt|;
if|if
condition|(
name|setup_prefix
operator|&&
name|prefix
operator|==
name|NULL
condition|)
name|prefix
operator|=
name|setup_git_directory
argument_list|()
expr_stmt|;
name|argv
operator|+=
name|tree_idx
expr_stmt|;
name|parse_treeish_arg
argument_list|(
name|argv
argument_list|,
operator|&
name|args
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|parse_pathspec_arg
argument_list|(
name|argv
operator|+
literal|1
argument_list|,
operator|&
name|args
argument_list|)
expr_stmt|;
return|return
name|ar
operator|->
name|write_archive
argument_list|(
operator|&
name|args
argument_list|)
return|;
block|}
end_function
end_unit
