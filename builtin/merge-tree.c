begin_unit
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"xdiff-interface.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_include
include|#
directive|include
file|"merge-blobs.h"
end_include
begin_decl_stmt
DECL|variable|merge_tree_usage
specifier|static
specifier|const
name|char
name|merge_tree_usage
index|[]
init|=
literal|"git merge-tree<base-tree><branch1><branch2>"
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|merge_list
struct|struct
name|merge_list
block|{
DECL|member|next
name|struct
name|merge_list
modifier|*
name|next
decl_stmt|;
DECL|member|link
name|struct
name|merge_list
modifier|*
name|link
decl_stmt|;
comment|/* other stages for this object */
DECL|member|stage
name|unsigned
name|int
name|stage
range|:
literal|2
decl_stmt|;
DECL|member|mode
name|unsigned
name|int
name|mode
decl_stmt|;
DECL|member|path
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
DECL|member|blob
name|struct
name|blob
modifier|*
name|blob
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|merge_result
DECL|variable|merge_result_end
specifier|static
name|struct
name|merge_list
modifier|*
name|merge_result
decl_stmt|,
modifier|*
modifier|*
name|merge_result_end
init|=
operator|&
name|merge_result
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_merge_entry
specifier|static
name|void
name|add_merge_entry
parameter_list|(
name|struct
name|merge_list
modifier|*
name|entry
parameter_list|)
block|{
operator|*
name|merge_result_end
operator|=
name|entry
expr_stmt|;
name|merge_result_end
operator|=
operator|&
name|entry
operator|->
name|next
expr_stmt|;
block|}
end_function
begin_function_decl
specifier|static
name|void
name|merge_trees
parameter_list|(
name|struct
name|tree_desc
name|t
index|[
literal|3
index|]
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|explanation
specifier|static
specifier|const
name|char
modifier|*
name|explanation
parameter_list|(
name|struct
name|merge_list
modifier|*
name|entry
parameter_list|)
block|{
switch|switch
condition|(
name|entry
operator|->
name|stage
condition|)
block|{
case|case
literal|0
case|:
return|return
literal|"merged"
return|;
case|case
literal|3
case|:
return|return
literal|"added in remote"
return|;
case|case
literal|2
case|:
if|if
condition|(
name|entry
operator|->
name|link
condition|)
return|return
literal|"added in both"
return|;
return|return
literal|"added in local"
return|;
block|}
comment|/* Existed in base */
name|entry
operator|=
name|entry
operator|->
name|link
expr_stmt|;
if|if
condition|(
operator|!
name|entry
condition|)
return|return
literal|"removed in both"
return|;
if|if
condition|(
name|entry
operator|->
name|link
condition|)
return|return
literal|"changed in both"
return|;
if|if
condition|(
name|entry
operator|->
name|stage
operator|==
literal|3
condition|)
return|return
literal|"removed in local"
return|;
return|return
literal|"removed in remote"
return|;
block|}
end_function
begin_function
DECL|function|result
specifier|static
name|void
modifier|*
name|result
parameter_list|(
name|struct
name|merge_list
modifier|*
name|entry
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
block|{
name|enum
name|object_type
name|type
decl_stmt|;
name|struct
name|blob
modifier|*
name|base
decl_stmt|,
modifier|*
name|our
decl_stmt|,
modifier|*
name|their
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
init|=
name|entry
operator|->
name|path
decl_stmt|;
if|if
condition|(
operator|!
name|entry
operator|->
name|stage
condition|)
return|return
name|read_sha1_file
argument_list|(
name|entry
operator|->
name|blob
operator|->
name|object
operator|.
name|oid
operator|.
name|hash
argument_list|,
operator|&
name|type
argument_list|,
name|size
argument_list|)
return|;
name|base
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|entry
operator|->
name|stage
operator|==
literal|1
condition|)
block|{
name|base
operator|=
name|entry
operator|->
name|blob
expr_stmt|;
name|entry
operator|=
name|entry
operator|->
name|link
expr_stmt|;
block|}
name|our
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|entry
operator|&&
name|entry
operator|->
name|stage
operator|==
literal|2
condition|)
block|{
name|our
operator|=
name|entry
operator|->
name|blob
expr_stmt|;
name|entry
operator|=
name|entry
operator|->
name|link
expr_stmt|;
block|}
name|their
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|entry
condition|)
name|their
operator|=
name|entry
operator|->
name|blob
expr_stmt|;
return|return
name|merge_blobs
argument_list|(
name|path
argument_list|,
name|base
argument_list|,
name|our
argument_list|,
name|their
argument_list|,
name|size
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|origin
specifier|static
name|void
modifier|*
name|origin
parameter_list|(
name|struct
name|merge_list
modifier|*
name|entry
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
block|{
name|enum
name|object_type
name|type
decl_stmt|;
while|while
condition|(
name|entry
condition|)
block|{
if|if
condition|(
name|entry
operator|->
name|stage
operator|==
literal|2
condition|)
return|return
name|read_sha1_file
argument_list|(
name|entry
operator|->
name|blob
operator|->
name|object
operator|.
name|oid
operator|.
name|hash
argument_list|,
operator|&
name|type
argument_list|,
name|size
argument_list|)
return|;
name|entry
operator|=
name|entry
operator|->
name|link
expr_stmt|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|show_outf
specifier|static
name|int
name|show_outf
parameter_list|(
name|void
modifier|*
name|priv_
parameter_list|,
name|mmbuffer_t
modifier|*
name|mb
parameter_list|,
name|int
name|nbuf
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nbuf
condition|;
name|i
operator|++
control|)
name|printf
argument_list|(
literal|"%.*s"
argument_list|,
operator|(
name|int
operator|)
name|mb
index|[
name|i
index|]
operator|.
name|size
argument_list|,
name|mb
index|[
name|i
index|]
operator|.
name|ptr
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|show_diff
specifier|static
name|void
name|show_diff
parameter_list|(
name|struct
name|merge_list
modifier|*
name|entry
parameter_list|)
block|{
name|unsigned
name|long
name|size
decl_stmt|;
name|mmfile_t
name|src
decl_stmt|,
name|dst
decl_stmt|;
name|xpparam_t
name|xpp
decl_stmt|;
name|xdemitconf_t
name|xecfg
decl_stmt|;
name|xdemitcb_t
name|ecb
decl_stmt|;
name|xpp
operator|.
name|flags
operator|=
literal|0
expr_stmt|;
name|memset
argument_list|(
operator|&
name|xecfg
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|xecfg
argument_list|)
argument_list|)
expr_stmt|;
name|xecfg
operator|.
name|ctxlen
operator|=
literal|3
expr_stmt|;
name|ecb
operator|.
name|outf
operator|=
name|show_outf
expr_stmt|;
name|ecb
operator|.
name|priv
operator|=
name|NULL
expr_stmt|;
name|src
operator|.
name|ptr
operator|=
name|origin
argument_list|(
name|entry
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|src
operator|.
name|ptr
condition|)
name|size
operator|=
literal|0
expr_stmt|;
name|src
operator|.
name|size
operator|=
name|size
expr_stmt|;
name|dst
operator|.
name|ptr
operator|=
name|result
argument_list|(
name|entry
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|dst
operator|.
name|ptr
condition|)
name|size
operator|=
literal|0
expr_stmt|;
name|dst
operator|.
name|size
operator|=
name|size
expr_stmt|;
if|if
condition|(
name|xdi_diff
argument_list|(
operator|&
name|src
argument_list|,
operator|&
name|dst
argument_list|,
operator|&
name|xpp
argument_list|,
operator|&
name|xecfg
argument_list|,
operator|&
name|ecb
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to generate diff"
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|src
operator|.
name|ptr
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|dst
operator|.
name|ptr
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_result_list
specifier|static
name|void
name|show_result_list
parameter_list|(
name|struct
name|merge_list
modifier|*
name|entry
parameter_list|)
block|{
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|explanation
argument_list|(
name|entry
argument_list|)
argument_list|)
expr_stmt|;
do|do
block|{
name|struct
name|merge_list
modifier|*
name|link
init|=
name|entry
operator|->
name|link
decl_stmt|;
specifier|static
specifier|const
name|char
modifier|*
name|desc
index|[
literal|4
index|]
init|=
block|{
literal|"result"
block|,
literal|"base"
block|,
literal|"our"
block|,
literal|"their"
block|}
decl_stmt|;
name|printf
argument_list|(
literal|"  %-6s %o %s %s\n"
argument_list|,
name|desc
index|[
name|entry
operator|->
name|stage
index|]
argument_list|,
name|entry
operator|->
name|mode
argument_list|,
name|oid_to_hex
argument_list|(
operator|&
name|entry
operator|->
name|blob
operator|->
name|object
operator|.
name|oid
argument_list|)
argument_list|,
name|entry
operator|->
name|path
argument_list|)
expr_stmt|;
name|entry
operator|=
name|link
expr_stmt|;
block|}
do|while
condition|(
name|entry
condition|)
do|;
block|}
end_function
begin_function
DECL|function|show_result
specifier|static
name|void
name|show_result
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|merge_list
modifier|*
name|walk
decl_stmt|;
name|walk
operator|=
name|merge_result
expr_stmt|;
while|while
condition|(
name|walk
condition|)
block|{
name|show_result_list
argument_list|(
name|walk
argument_list|)
expr_stmt|;
name|show_diff
argument_list|(
name|walk
argument_list|)
expr_stmt|;
name|walk
operator|=
name|walk
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/* An empty entry never compares same, not even to another empty entry */
end_comment
begin_function
DECL|function|same_entry
specifier|static
name|int
name|same_entry
parameter_list|(
name|struct
name|name_entry
modifier|*
name|a
parameter_list|,
name|struct
name|name_entry
modifier|*
name|b
parameter_list|)
block|{
return|return
name|a
operator|->
name|sha1
operator|&&
name|b
operator|->
name|sha1
operator|&&
operator|!
name|hashcmp
argument_list|(
name|a
operator|->
name|sha1
argument_list|,
name|b
operator|->
name|sha1
argument_list|)
operator|&&
name|a
operator|->
name|mode
operator|==
name|b
operator|->
name|mode
return|;
block|}
end_function
begin_function
DECL|function|both_empty
specifier|static
name|int
name|both_empty
parameter_list|(
name|struct
name|name_entry
modifier|*
name|a
parameter_list|,
name|struct
name|name_entry
modifier|*
name|b
parameter_list|)
block|{
return|return
operator|!
operator|(
name|a
operator|->
name|sha1
operator|||
name|b
operator|->
name|sha1
operator|)
return|;
block|}
end_function
begin_function
DECL|function|create_entry
specifier|static
name|struct
name|merge_list
modifier|*
name|create_entry
parameter_list|(
name|unsigned
name|stage
parameter_list|,
name|unsigned
name|mode
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|struct
name|merge_list
modifier|*
name|res
init|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|res
argument_list|)
argument_list|)
decl_stmt|;
name|res
operator|->
name|stage
operator|=
name|stage
expr_stmt|;
name|res
operator|->
name|path
operator|=
name|path
expr_stmt|;
name|res
operator|->
name|mode
operator|=
name|mode
expr_stmt|;
name|res
operator|->
name|blob
operator|=
name|lookup_blob
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
return|return
name|res
return|;
block|}
end_function
begin_function
DECL|function|traverse_path
specifier|static
name|char
modifier|*
name|traverse_path
parameter_list|(
specifier|const
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|,
specifier|const
name|struct
name|name_entry
modifier|*
name|n
parameter_list|)
block|{
name|char
modifier|*
name|path
init|=
name|xmallocz
argument_list|(
name|traverse_path_len
argument_list|(
name|info
argument_list|,
name|n
argument_list|)
argument_list|)
decl_stmt|;
return|return
name|make_traverse_path
argument_list|(
name|path
argument_list|,
name|info
argument_list|,
name|n
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|resolve
specifier|static
name|void
name|resolve
parameter_list|(
specifier|const
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|,
name|struct
name|name_entry
modifier|*
name|ours
parameter_list|,
name|struct
name|name_entry
modifier|*
name|result
parameter_list|)
block|{
name|struct
name|merge_list
modifier|*
name|orig
decl_stmt|,
modifier|*
name|final
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
comment|/* If it's already ours, don't bother showing it */
if|if
condition|(
operator|!
name|ours
condition|)
return|return;
name|path
operator|=
name|traverse_path
argument_list|(
name|info
argument_list|,
name|result
argument_list|)
expr_stmt|;
name|orig
operator|=
name|create_entry
argument_list|(
literal|2
argument_list|,
name|ours
operator|->
name|mode
argument_list|,
name|ours
operator|->
name|sha1
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|final
operator|=
name|create_entry
argument_list|(
literal|0
argument_list|,
name|result
operator|->
name|mode
argument_list|,
name|result
operator|->
name|sha1
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|final
operator|->
name|link
operator|=
name|orig
expr_stmt|;
name|add_merge_entry
argument_list|(
name|final
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|unresolved_directory
specifier|static
name|void
name|unresolved_directory
parameter_list|(
specifier|const
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|,
name|struct
name|name_entry
name|n
index|[
literal|3
index|]
parameter_list|)
block|{
name|char
modifier|*
name|newbase
decl_stmt|;
name|struct
name|name_entry
modifier|*
name|p
decl_stmt|;
name|struct
name|tree_desc
name|t
index|[
literal|3
index|]
decl_stmt|;
name|void
modifier|*
name|buf0
decl_stmt|,
modifier|*
name|buf1
decl_stmt|,
modifier|*
name|buf2
decl_stmt|;
for|for
control|(
name|p
operator|=
name|n
init|;
name|p
operator|<
name|n
operator|+
literal|3
condition|;
name|p
operator|++
control|)
block|{
if|if
condition|(
name|p
operator|->
name|mode
operator|&&
name|S_ISDIR
argument_list|(
name|p
operator|->
name|mode
argument_list|)
condition|)
break|break;
block|}
if|if
condition|(
name|n
operator|+
literal|3
operator|<=
name|p
condition|)
return|return;
comment|/* there is no tree here */
name|newbase
operator|=
name|traverse_path
argument_list|(
name|info
argument_list|,
name|p
argument_list|)
expr_stmt|;
DECL|macro|ENTRY_SHA1
define|#
directive|define
name|ENTRY_SHA1
parameter_list|(
name|e
parameter_list|)
value|(((e)->mode&& S_ISDIR((e)->mode)) ? (e)->sha1 : NULL)
name|buf0
operator|=
name|fill_tree_descriptor
argument_list|(
name|t
operator|+
literal|0
argument_list|,
name|ENTRY_SHA1
argument_list|(
name|n
operator|+
literal|0
argument_list|)
argument_list|)
expr_stmt|;
name|buf1
operator|=
name|fill_tree_descriptor
argument_list|(
name|t
operator|+
literal|1
argument_list|,
name|ENTRY_SHA1
argument_list|(
name|n
operator|+
literal|1
argument_list|)
argument_list|)
expr_stmt|;
name|buf2
operator|=
name|fill_tree_descriptor
argument_list|(
name|t
operator|+
literal|2
argument_list|,
name|ENTRY_SHA1
argument_list|(
name|n
operator|+
literal|2
argument_list|)
argument_list|)
expr_stmt|;
DECL|macro|ENTRY_SHA1
undef|#
directive|undef
name|ENTRY_SHA1
name|merge_trees
argument_list|(
name|t
argument_list|,
name|newbase
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf0
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf2
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|newbase
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|link_entry
specifier|static
name|struct
name|merge_list
modifier|*
name|link_entry
parameter_list|(
name|unsigned
name|stage
parameter_list|,
specifier|const
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|,
name|struct
name|name_entry
modifier|*
name|n
parameter_list|,
name|struct
name|merge_list
modifier|*
name|entry
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
name|struct
name|merge_list
modifier|*
name|link
decl_stmt|;
if|if
condition|(
operator|!
name|n
operator|->
name|mode
condition|)
return|return
name|entry
return|;
if|if
condition|(
name|entry
condition|)
name|path
operator|=
name|entry
operator|->
name|path
expr_stmt|;
else|else
name|path
operator|=
name|traverse_path
argument_list|(
name|info
argument_list|,
name|n
argument_list|)
expr_stmt|;
name|link
operator|=
name|create_entry
argument_list|(
name|stage
argument_list|,
name|n
operator|->
name|mode
argument_list|,
name|n
operator|->
name|sha1
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|link
operator|->
name|link
operator|=
name|entry
expr_stmt|;
return|return
name|link
return|;
block|}
end_function
begin_function
DECL|function|unresolved
specifier|static
name|void
name|unresolved
parameter_list|(
specifier|const
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|,
name|struct
name|name_entry
name|n
index|[
literal|3
index|]
parameter_list|)
block|{
name|struct
name|merge_list
modifier|*
name|entry
init|=
name|NULL
decl_stmt|;
name|int
name|i
decl_stmt|;
name|unsigned
name|dirmask
init|=
literal|0
decl_stmt|,
name|mask
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|3
condition|;
name|i
operator|++
control|)
block|{
name|mask
operator||=
operator|(
literal|1
operator|<<
name|i
operator|)
expr_stmt|;
comment|/* 		 * Treat missing entries as directories so that we return 		 * after unresolved_directory has handled this. 		 */
if|if
condition|(
operator|!
name|n
index|[
name|i
index|]
operator|.
name|mode
operator|||
name|S_ISDIR
argument_list|(
name|n
index|[
name|i
index|]
operator|.
name|mode
argument_list|)
condition|)
name|dirmask
operator||=
operator|(
literal|1
operator|<<
name|i
operator|)
expr_stmt|;
block|}
name|unresolved_directory
argument_list|(
name|info
argument_list|,
name|n
argument_list|)
expr_stmt|;
if|if
condition|(
name|dirmask
operator|==
name|mask
condition|)
return|return;
if|if
condition|(
name|n
index|[
literal|2
index|]
operator|.
name|mode
operator|&&
operator|!
name|S_ISDIR
argument_list|(
name|n
index|[
literal|2
index|]
operator|.
name|mode
argument_list|)
condition|)
name|entry
operator|=
name|link_entry
argument_list|(
literal|3
argument_list|,
name|info
argument_list|,
name|n
operator|+
literal|2
argument_list|,
name|entry
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
index|[
literal|1
index|]
operator|.
name|mode
operator|&&
operator|!
name|S_ISDIR
argument_list|(
name|n
index|[
literal|1
index|]
operator|.
name|mode
argument_list|)
condition|)
name|entry
operator|=
name|link_entry
argument_list|(
literal|2
argument_list|,
name|info
argument_list|,
name|n
operator|+
literal|1
argument_list|,
name|entry
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
index|[
literal|0
index|]
operator|.
name|mode
operator|&&
operator|!
name|S_ISDIR
argument_list|(
name|n
index|[
literal|0
index|]
operator|.
name|mode
argument_list|)
condition|)
name|entry
operator|=
name|link_entry
argument_list|(
literal|1
argument_list|,
name|info
argument_list|,
name|n
operator|+
literal|0
argument_list|,
name|entry
argument_list|)
expr_stmt|;
name|add_merge_entry
argument_list|(
name|entry
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Merge two trees together (t[1] and t[2]), using a common base (t[0])  * as the origin.  *  * This walks the (sorted) trees in lock-step, checking every possible  * name. Note that directories automatically sort differently from other  * files (see "base_name_compare"), so you'll never see file/directory  * conflicts, because they won't ever compare the same.  *  * IOW, if a directory changes to a filename, it will automatically be  * seen as the directory going away, and the filename being created.  *  * Think of this as a three-way diff.  *  * The output will be either:  *  - successful merge  *	 "0 mode sha1 filename"  *    NOTE NOTE NOTE! FIXME! We really really need to walk the index  *    in parallel with this too!  *  *  - conflict:  *	"1 mode sha1 filename"  *	"2 mode sha1 filename"  *	"3 mode sha1 filename"  *    where not all of the 1/2/3 lines may exist, of course.  *  * The successful merge rules are the same as for the three-way merge  * in git-read-tree.  */
end_comment
begin_function
DECL|function|threeway_callback
specifier|static
name|int
name|threeway_callback
parameter_list|(
name|int
name|n
parameter_list|,
name|unsigned
name|long
name|mask
parameter_list|,
name|unsigned
name|long
name|dirmask
parameter_list|,
name|struct
name|name_entry
modifier|*
name|entry
parameter_list|,
name|struct
name|traverse_info
modifier|*
name|info
parameter_list|)
block|{
comment|/* Same in both? */
if|if
condition|(
name|same_entry
argument_list|(
name|entry
operator|+
literal|1
argument_list|,
name|entry
operator|+
literal|2
argument_list|)
operator|||
name|both_empty
argument_list|(
name|entry
operator|+
literal|1
argument_list|,
name|entry
operator|+
literal|2
argument_list|)
condition|)
block|{
comment|/* Modified, added or removed identically */
name|resolve
argument_list|(
name|info
argument_list|,
name|NULL
argument_list|,
name|entry
operator|+
literal|1
argument_list|)
expr_stmt|;
return|return
name|mask
return|;
block|}
if|if
condition|(
name|same_entry
argument_list|(
name|entry
operator|+
literal|0
argument_list|,
name|entry
operator|+
literal|1
argument_list|)
condition|)
block|{
if|if
condition|(
name|entry
index|[
literal|2
index|]
operator|.
name|sha1
operator|&&
operator|!
name|S_ISDIR
argument_list|(
name|entry
index|[
literal|2
index|]
operator|.
name|mode
argument_list|)
condition|)
block|{
comment|/* We did not touch, they modified -- take theirs */
name|resolve
argument_list|(
name|info
argument_list|,
name|entry
operator|+
literal|1
argument_list|,
name|entry
operator|+
literal|2
argument_list|)
expr_stmt|;
return|return
name|mask
return|;
block|}
comment|/* 		 * If we did not touch a directory but they made it 		 * into a file, we fall through and unresolved() 		 * recurses down.  Likewise for the opposite case. 		 */
block|}
if|if
condition|(
name|same_entry
argument_list|(
name|entry
operator|+
literal|0
argument_list|,
name|entry
operator|+
literal|2
argument_list|)
operator|||
name|both_empty
argument_list|(
name|entry
operator|+
literal|0
argument_list|,
name|entry
operator|+
literal|2
argument_list|)
condition|)
block|{
comment|/* We added, modified or removed, they did not touch -- take ours */
name|resolve
argument_list|(
name|info
argument_list|,
name|NULL
argument_list|,
name|entry
operator|+
literal|1
argument_list|)
expr_stmt|;
return|return
name|mask
return|;
block|}
name|unresolved
argument_list|(
name|info
argument_list|,
name|entry
argument_list|)
expr_stmt|;
return|return
name|mask
return|;
block|}
end_function
begin_function
DECL|function|merge_trees
specifier|static
name|void
name|merge_trees
parameter_list|(
name|struct
name|tree_desc
name|t
index|[
literal|3
index|]
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|struct
name|traverse_info
name|info
decl_stmt|;
name|setup_traverse_info
argument_list|(
operator|&
name|info
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|info
operator|.
name|fn
operator|=
name|threeway_callback
expr_stmt|;
name|traverse_trees
argument_list|(
literal|3
argument_list|,
name|t
argument_list|,
operator|&
name|info
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_tree_descriptor
specifier|static
name|void
modifier|*
name|get_tree_descriptor
parameter_list|(
name|struct
name|tree_desc
modifier|*
name|desc
parameter_list|,
specifier|const
name|char
modifier|*
name|rev
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|void
modifier|*
name|buf
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|rev
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"unknown rev %s"
argument_list|,
name|rev
argument_list|)
expr_stmt|;
name|buf
operator|=
name|fill_tree_descriptor
argument_list|(
name|desc
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
condition|)
name|die
argument_list|(
literal|"%s is not a tree"
argument_list|,
name|rev
argument_list|)
expr_stmt|;
return|return
name|buf
return|;
block|}
end_function
begin_function
DECL|function|cmd_merge_tree
name|int
name|cmd_merge_tree
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
parameter_list|)
block|{
name|struct
name|tree_desc
name|t
index|[
literal|3
index|]
decl_stmt|;
name|void
modifier|*
name|buf1
decl_stmt|,
modifier|*
name|buf2
decl_stmt|,
modifier|*
name|buf3
decl_stmt|;
if|if
condition|(
name|argc
operator|!=
literal|4
condition|)
name|usage
argument_list|(
name|merge_tree_usage
argument_list|)
expr_stmt|;
name|buf1
operator|=
name|get_tree_descriptor
argument_list|(
name|t
operator|+
literal|0
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|buf2
operator|=
name|get_tree_descriptor
argument_list|(
name|t
operator|+
literal|1
argument_list|,
name|argv
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
name|buf3
operator|=
name|get_tree_descriptor
argument_list|(
name|t
operator|+
literal|2
argument_list|,
name|argv
index|[
literal|3
index|]
argument_list|)
expr_stmt|;
name|merge_trees
argument_list|(
name|t
argument_list|,
literal|""
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf2
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf3
argument_list|)
expr_stmt|;
name|show_result
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
