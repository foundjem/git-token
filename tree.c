begin_unit
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_decl_stmt
DECL|variable|tree_type
specifier|const
name|char
modifier|*
name|tree_type
init|=
literal|"tree"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|read_one_entry
specifier|static
name|int
name|read_one_entry
parameter_list|(
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
name|pathname
parameter_list|,
name|unsigned
name|mode
parameter_list|,
name|int
name|stage
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|pathname
argument_list|)
decl_stmt|;
name|unsigned
name|int
name|size
init|=
name|cache_entry_size
argument_list|(
name|baselen
operator|+
name|len
argument_list|)
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|xmalloc
argument_list|(
name|size
argument_list|)
decl_stmt|;
name|memset
argument_list|(
name|ce
argument_list|,
literal|0
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_mode
operator|=
name|create_ce_mode
argument_list|(
name|mode
argument_list|)
expr_stmt|;
name|ce
operator|->
name|ce_flags
operator|=
name|create_ce_flags
argument_list|(
name|baselen
operator|+
name|len
argument_list|,
name|stage
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|name
operator|+
name|baselen
argument_list|,
name|pathname
argument_list|,
name|len
operator|+
literal|1
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
name|add_cache_entry
argument_list|(
name|ce
argument_list|,
name|ADD_CACHE_OK_TO_ADD
operator||
name|ADD_CACHE_SKIP_DFCHECK
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|match_tree_entry
specifier|static
name|int
name|match_tree_entry
parameter_list|(
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
name|path
parameter_list|,
name|unsigned
name|int
name|mode
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|paths
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|match
decl_stmt|;
name|int
name|pathlen
decl_stmt|;
if|if
condition|(
operator|!
name|paths
condition|)
return|return
literal|1
return|;
name|pathlen
operator|=
name|strlen
argument_list|(
name|path
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|match
operator|=
operator|*
name|paths
operator|++
operator|)
operator|!=
name|NULL
condition|)
block|{
name|int
name|matchlen
init|=
name|strlen
argument_list|(
name|match
argument_list|)
decl_stmt|;
if|if
condition|(
name|baselen
operator|>=
name|matchlen
condition|)
block|{
comment|/* If it doesn't match, move along... */
if|if
condition|(
name|strncmp
argument_list|(
name|base
argument_list|,
name|match
argument_list|,
name|matchlen
argument_list|)
condition|)
continue|continue;
comment|/* The base is a subdirectory of a path which was specified. */
return|return
literal|1
return|;
block|}
comment|/* Does the base match? */
if|if
condition|(
name|strncmp
argument_list|(
name|base
argument_list|,
name|match
argument_list|,
name|baselen
argument_list|)
condition|)
continue|continue;
name|match
operator|+=
name|baselen
expr_stmt|;
name|matchlen
operator|-=
name|baselen
expr_stmt|;
if|if
condition|(
name|pathlen
operator|>
name|matchlen
condition|)
continue|continue;
if|if
condition|(
name|matchlen
operator|>
name|pathlen
condition|)
block|{
if|if
condition|(
name|match
index|[
name|pathlen
index|]
operator|!=
literal|'/'
condition|)
continue|continue;
if|if
condition|(
operator|!
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
continue|continue;
block|}
if|if
condition|(
name|strncmp
argument_list|(
name|path
argument_list|,
name|match
argument_list|,
name|pathlen
argument_list|)
condition|)
continue|continue;
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_tree_recursive
specifier|static
name|int
name|read_tree_recursive
parameter_list|(
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
name|int
name|baselen
parameter_list|,
name|int
name|stage
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|match
parameter_list|)
block|{
while|while
condition|(
name|size
condition|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|buffer
argument_list|)
operator|+
literal|1
decl_stmt|;
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|buffer
operator|+
name|len
decl_stmt|;
name|char
modifier|*
name|path
init|=
name|strchr
argument_list|(
name|buffer
argument_list|,
literal|' '
argument_list|)
operator|+
literal|1
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
if|if
condition|(
name|size
operator|<
name|len
operator|+
literal|20
operator|||
name|sscanf
argument_list|(
name|buffer
argument_list|,
literal|"%o"
argument_list|,
operator|&
name|mode
argument_list|)
operator|!=
literal|1
condition|)
return|return
operator|-
literal|1
return|;
name|buffer
operator|=
name|sha1
operator|+
literal|20
expr_stmt|;
name|size
operator|-=
name|len
operator|+
literal|20
expr_stmt|;
if|if
condition|(
operator|!
name|match_tree_entry
argument_list|(
name|base
argument_list|,
name|baselen
argument_list|,
name|path
argument_list|,
name|mode
argument_list|,
name|match
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
condition|)
block|{
name|int
name|retval
decl_stmt|;
name|int
name|pathlen
init|=
name|strlen
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|char
modifier|*
name|newbase
decl_stmt|;
name|void
modifier|*
name|eltbuf
decl_stmt|;
name|char
name|elttype
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|eltsize
decl_stmt|;
name|eltbuf
operator|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|elttype
argument_list|,
operator|&
name|eltsize
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|eltbuf
operator|||
name|strcmp
argument_list|(
name|elttype
argument_list|,
literal|"tree"
argument_list|)
condition|)
block|{
if|if
condition|(
name|eltbuf
condition|)
name|free
argument_list|(
name|eltbuf
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|newbase
operator|=
name|xmalloc
argument_list|(
name|baselen
operator|+
literal|1
operator|+
name|pathlen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|newbase
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|newbase
operator|+
name|baselen
argument_list|,
name|path
argument_list|,
name|pathlen
argument_list|)
expr_stmt|;
name|newbase
index|[
name|baselen
operator|+
name|pathlen
index|]
operator|=
literal|'/'
expr_stmt|;
name|retval
operator|=
name|read_tree_recursive
argument_list|(
name|eltbuf
argument_list|,
name|eltsize
argument_list|,
name|newbase
argument_list|,
name|baselen
operator|+
name|pathlen
operator|+
literal|1
argument_list|,
name|stage
argument_list|,
name|match
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|eltbuf
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|newbase
argument_list|)
expr_stmt|;
if|if
condition|(
name|retval
condition|)
return|return
operator|-
literal|1
return|;
continue|continue;
block|}
if|if
condition|(
name|read_one_entry
argument_list|(
name|sha1
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|path
argument_list|,
name|mode
argument_list|,
name|stage
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_tree
name|int
name|read_tree
parameter_list|(
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|int
name|stage
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|match
parameter_list|)
block|{
return|return
name|read_tree_recursive
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|,
literal|""
argument_list|,
literal|0
argument_list|,
name|stage
argument_list|,
name|match
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|lookup_tree
name|struct
name|tree
modifier|*
name|lookup_tree
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|lookup_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
block|{
name|struct
name|tree
modifier|*
name|ret
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|tree
argument_list|)
argument_list|)
decl_stmt|;
name|memset
argument_list|(
name|ret
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|tree
argument_list|)
argument_list|)
expr_stmt|;
name|created_object
argument_list|(
name|sha1
argument_list|,
operator|&
name|ret
operator|->
name|object
argument_list|)
expr_stmt|;
name|ret
operator|->
name|object
operator|.
name|type
operator|=
name|tree_type
expr_stmt|;
return|return
name|ret
return|;
block|}
if|if
condition|(
operator|!
name|obj
operator|->
name|type
condition|)
name|obj
operator|->
name|type
operator|=
name|tree_type
expr_stmt|;
if|if
condition|(
name|obj
operator|->
name|type
operator|!=
name|tree_type
condition|)
block|{
name|error
argument_list|(
literal|"Object %s is a %s, not a tree"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|obj
operator|->
name|type
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
return|return
operator|(
expr|struct
name|tree
operator|*
operator|)
name|obj
return|;
block|}
end_function
begin_function
DECL|function|parse_tree_buffer
name|int
name|parse_tree_buffer
parameter_list|(
name|struct
name|tree
modifier|*
name|item
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|void
modifier|*
name|bufptr
init|=
name|buffer
decl_stmt|;
name|struct
name|tree_entry_list
modifier|*
modifier|*
name|list_p
decl_stmt|;
if|if
condition|(
name|item
operator|->
name|object
operator|.
name|parsed
condition|)
return|return
literal|0
return|;
name|item
operator|->
name|object
operator|.
name|parsed
operator|=
literal|1
expr_stmt|;
name|list_p
operator|=
operator|&
name|item
operator|->
name|entries
expr_stmt|;
while|while
condition|(
name|size
condition|)
block|{
name|struct
name|object
modifier|*
name|obj
decl_stmt|;
name|struct
name|tree_entry_list
modifier|*
name|entry
decl_stmt|;
name|int
name|len
init|=
literal|1
operator|+
name|strlen
argument_list|(
name|bufptr
argument_list|)
decl_stmt|;
name|unsigned
name|char
modifier|*
name|file_sha1
init|=
name|bufptr
operator|+
name|len
decl_stmt|;
name|char
modifier|*
name|path
init|=
name|strchr
argument_list|(
name|bufptr
argument_list|,
literal|' '
argument_list|)
decl_stmt|;
name|unsigned
name|int
name|mode
decl_stmt|;
if|if
condition|(
name|size
operator|<
name|len
operator|+
literal|20
operator|||
operator|!
name|path
operator|||
name|sscanf
argument_list|(
name|bufptr
argument_list|,
literal|"%o"
argument_list|,
operator|&
name|mode
argument_list|)
operator|!=
literal|1
condition|)
return|return
operator|-
literal|1
return|;
name|entry
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|tree_entry_list
argument_list|)
argument_list|)
expr_stmt|;
name|entry
operator|->
name|name
operator|=
name|strdup
argument_list|(
name|path
operator|+
literal|1
argument_list|)
expr_stmt|;
name|entry
operator|->
name|directory
operator|=
name|S_ISDIR
argument_list|(
name|mode
argument_list|)
operator|!=
literal|0
expr_stmt|;
name|entry
operator|->
name|executable
operator|=
operator|(
name|mode
operator|&
name|S_IXUSR
operator|)
operator|!=
literal|0
expr_stmt|;
name|entry
operator|->
name|symlink
operator|=
name|S_ISLNK
argument_list|(
name|mode
argument_list|)
operator|!=
literal|0
expr_stmt|;
name|entry
operator|->
name|zeropad
operator|=
operator|*
operator|(
name|char
operator|*
operator|)
name|bufptr
operator|==
literal|'0'
expr_stmt|;
name|entry
operator|->
name|mode
operator|=
name|mode
expr_stmt|;
name|entry
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
name|bufptr
operator|+=
name|len
operator|+
literal|20
expr_stmt|;
name|size
operator|-=
name|len
operator|+
literal|20
expr_stmt|;
if|if
condition|(
name|entry
operator|->
name|directory
condition|)
block|{
name|entry
operator|->
name|item
operator|.
name|tree
operator|=
name|lookup_tree
argument_list|(
name|file_sha1
argument_list|)
expr_stmt|;
name|obj
operator|=
operator|&
name|entry
operator|->
name|item
operator|.
name|tree
operator|->
name|object
expr_stmt|;
block|}
else|else
block|{
name|entry
operator|->
name|item
operator|.
name|blob
operator|=
name|lookup_blob
argument_list|(
name|file_sha1
argument_list|)
expr_stmt|;
name|obj
operator|=
operator|&
name|entry
operator|->
name|item
operator|.
name|blob
operator|->
name|object
expr_stmt|;
block|}
if|if
condition|(
name|obj
condition|)
name|add_ref
argument_list|(
operator|&
name|item
operator|->
name|object
argument_list|,
name|obj
argument_list|)
expr_stmt|;
name|entry
operator|->
name|parent
operator|=
name|NULL
expr_stmt|;
comment|/* needs to be filled by the user */
operator|*
name|list_p
operator|=
name|entry
expr_stmt|;
name|list_p
operator|=
operator|&
name|entry
operator|->
name|next
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_tree
name|int
name|parse_tree
parameter_list|(
name|struct
name|tree
modifier|*
name|item
parameter_list|)
block|{
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|void
modifier|*
name|buffer
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|int
name|ret
decl_stmt|;
if|if
condition|(
name|item
operator|->
name|object
operator|.
name|parsed
condition|)
return|return
literal|0
return|;
name|buffer
operator|=
name|read_sha1_file
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
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
literal|"Could not read %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|strcmp
argument_list|(
name|type
argument_list|,
name|tree_type
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"Object %s not a tree"
argument_list|,
name|sha1_to_hex
argument_list|(
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
return|;
block|}
name|ret
operator|=
name|parse_tree_buffer
argument_list|(
name|item
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
name|ret
return|;
block|}
end_function
end_unit
