begin_unit
begin_comment
comment|/*  * Builtin "git diff"  *  * Copyright (c) 2006 Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"lockfile.h"
end_include
begin_include
include|#
directive|include
file|"color.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"diffcore.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"log-tree.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"submodule.h"
end_include
begin_include
include|#
directive|include
file|"sha1-array.h"
end_include
begin_define
DECL|macro|DIFF_NO_INDEX_EXPLICIT
define|#
directive|define
name|DIFF_NO_INDEX_EXPLICIT
value|1
end_define
begin_define
DECL|macro|DIFF_NO_INDEX_IMPLICIT
define|#
directive|define
name|DIFF_NO_INDEX_IMPLICIT
value|2
end_define
begin_struct
DECL|struct|blobinfo
struct|struct
name|blobinfo
block|{
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|mode
name|unsigned
name|mode
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|builtin_diff_usage
specifier|static
specifier|const
name|char
name|builtin_diff_usage
index|[]
init|=
literal|"git diff [<options>] [<commit> [<commit>]] [--] [<path>...]"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|stuff_change
specifier|static
name|void
name|stuff_change
parameter_list|(
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|,
name|unsigned
name|old_mode
parameter_list|,
name|unsigned
name|new_mode
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|old_sha1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|new_sha1
parameter_list|,
name|int
name|old_sha1_valid
parameter_list|,
name|int
name|new_sha1_valid
parameter_list|,
specifier|const
name|char
modifier|*
name|old_name
parameter_list|,
specifier|const
name|char
modifier|*
name|new_name
parameter_list|)
block|{
name|struct
name|diff_filespec
modifier|*
name|one
decl_stmt|,
modifier|*
name|two
decl_stmt|;
if|if
condition|(
operator|!
name|is_null_sha1
argument_list|(
name|old_sha1
argument_list|)
operator|&&
operator|!
name|is_null_sha1
argument_list|(
name|new_sha1
argument_list|)
operator|&&
operator|!
name|hashcmp
argument_list|(
name|old_sha1
argument_list|,
name|new_sha1
argument_list|)
operator|&&
operator|(
name|old_mode
operator|==
name|new_mode
operator|)
condition|)
return|return;
if|if
condition|(
name|DIFF_OPT_TST
argument_list|(
name|opt
argument_list|,
name|REVERSE_DIFF
argument_list|)
condition|)
block|{
name|unsigned
name|tmp
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|tmp_u
decl_stmt|;
specifier|const
name|char
modifier|*
name|tmp_c
decl_stmt|;
name|tmp
operator|=
name|old_mode
expr_stmt|;
name|old_mode
operator|=
name|new_mode
expr_stmt|;
name|new_mode
operator|=
name|tmp
expr_stmt|;
name|tmp_u
operator|=
name|old_sha1
expr_stmt|;
name|old_sha1
operator|=
name|new_sha1
expr_stmt|;
name|new_sha1
operator|=
name|tmp_u
expr_stmt|;
name|tmp_c
operator|=
name|old_name
expr_stmt|;
name|old_name
operator|=
name|new_name
expr_stmt|;
name|new_name
operator|=
name|tmp_c
expr_stmt|;
block|}
if|if
condition|(
name|opt
operator|->
name|prefix
operator|&&
operator|(
name|strncmp
argument_list|(
name|old_name
argument_list|,
name|opt
operator|->
name|prefix
argument_list|,
name|opt
operator|->
name|prefix_length
argument_list|)
operator|||
name|strncmp
argument_list|(
name|new_name
argument_list|,
name|opt
operator|->
name|prefix
argument_list|,
name|opt
operator|->
name|prefix_length
argument_list|)
operator|)
condition|)
return|return;
name|one
operator|=
name|alloc_filespec
argument_list|(
name|old_name
argument_list|)
expr_stmt|;
name|two
operator|=
name|alloc_filespec
argument_list|(
name|new_name
argument_list|)
expr_stmt|;
name|fill_filespec
argument_list|(
name|one
argument_list|,
name|old_sha1
argument_list|,
name|old_sha1_valid
argument_list|,
name|old_mode
argument_list|)
expr_stmt|;
name|fill_filespec
argument_list|(
name|two
argument_list|,
name|new_sha1
argument_list|,
name|new_sha1_valid
argument_list|,
name|new_mode
argument_list|)
expr_stmt|;
name|diff_queue
argument_list|(
operator|&
name|diff_queued_diff
argument_list|,
name|one
argument_list|,
name|two
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|builtin_diff_b_f
specifier|static
name|int
name|builtin_diff_b_f
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|struct
name|blobinfo
modifier|*
name|blob
parameter_list|)
block|{
comment|/* Blob vs file in the working tree*/
name|struct
name|stat
name|st
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
decl_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
name|GUARD_PATHSPEC
argument_list|(
operator|&
name|revs
operator|->
name|prune_data
argument_list|,
name|PATHSPEC_FROMTOP
operator||
name|PATHSPEC_LITERAL
argument_list|)
expr_stmt|;
name|path
operator|=
name|revs
operator|->
name|prune_data
operator|.
name|items
index|[
literal|0
index|]
operator|.
name|match
expr_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"failed to stat '%s'"
argument_list|)
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|||
name|S_ISLNK
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"'%s': not a regular file or symlink"
argument_list|)
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|diff_set_mnemonic_prefix
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
literal|"o/"
argument_list|,
literal|"w/"
argument_list|)
expr_stmt|;
if|if
condition|(
name|blob
index|[
literal|0
index|]
operator|.
name|mode
operator|==
name|S_IFINVALID
condition|)
name|blob
index|[
literal|0
index|]
operator|.
name|mode
operator|=
name|canon_mode
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
expr_stmt|;
name|stuff_change
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|blob
index|[
literal|0
index|]
operator|.
name|mode
argument_list|,
name|canon_mode
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
argument_list|,
name|blob
index|[
literal|0
index|]
operator|.
name|sha1
argument_list|,
name|null_sha1
argument_list|,
literal|1
argument_list|,
literal|0
argument_list|,
name|path
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|diffcore_std
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|builtin_diff_blobs
specifier|static
name|int
name|builtin_diff_blobs
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|struct
name|blobinfo
modifier|*
name|blob
parameter_list|)
block|{
name|unsigned
name|mode
init|=
name|canon_mode
argument_list|(
name|S_IFREG
operator||
literal|0644
argument_list|)
decl_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|blob
index|[
literal|0
index|]
operator|.
name|mode
operator|==
name|S_IFINVALID
condition|)
name|blob
index|[
literal|0
index|]
operator|.
name|mode
operator|=
name|mode
expr_stmt|;
if|if
condition|(
name|blob
index|[
literal|1
index|]
operator|.
name|mode
operator|==
name|S_IFINVALID
condition|)
name|blob
index|[
literal|1
index|]
operator|.
name|mode
operator|=
name|mode
expr_stmt|;
name|stuff_change
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|blob
index|[
literal|0
index|]
operator|.
name|mode
argument_list|,
name|blob
index|[
literal|1
index|]
operator|.
name|mode
argument_list|,
name|blob
index|[
literal|0
index|]
operator|.
name|sha1
argument_list|,
name|blob
index|[
literal|1
index|]
operator|.
name|sha1
argument_list|,
literal|1
argument_list|,
literal|1
argument_list|,
name|blob
index|[
literal|0
index|]
operator|.
name|name
argument_list|,
name|blob
index|[
literal|1
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
name|diffcore_std
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|builtin_diff_index
specifier|static
name|int
name|builtin_diff_index
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
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
name|cached
init|=
literal|0
decl_stmt|;
while|while
condition|(
literal|1
operator|<
name|argc
condition|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
literal|1
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--cached"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--staged"
argument_list|)
condition|)
name|cached
operator|=
literal|1
expr_stmt|;
else|else
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
comment|/* 	 * Make sure there is one revision (i.e. pending object), 	 * and there is no revision filtering parameters. 	 */
if|if
condition|(
name|revs
operator|->
name|pending
operator|.
name|nr
operator|!=
literal|1
operator|||
name|revs
operator|->
name|max_count
operator|!=
operator|-
literal|1
operator|||
name|revs
operator|->
name|min_age
operator|!=
operator|-
literal|1
operator|||
name|revs
operator|->
name|max_age
operator|!=
operator|-
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|cached
condition|)
block|{
name|setup_work_tree
argument_list|()
expr_stmt|;
if|if
condition|(
name|read_cache_preload
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
operator|.
name|pathspec
argument_list|)
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
literal|"read_cache_preload"
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
block|}
elseif|else
if|if
condition|(
name|read_cache
argument_list|()
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
literal|"read_cache"
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
name|run_diff_index
argument_list|(
name|revs
argument_list|,
name|cached
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|builtin_diff_tree
specifier|static
name|int
name|builtin_diff_tree
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|struct
name|object_array_entry
modifier|*
name|ent0
parameter_list|,
name|struct
name|object_array_entry
modifier|*
name|ent1
parameter_list|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
argument_list|(
name|sha1
index|[
literal|2
index|]
argument_list|)
decl_stmt|;
name|int
name|swap
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
comment|/* 	 * We saw two trees, ent0 and ent1.  If ent1 is uninteresting, 	 * swap them. 	 */
if|if
condition|(
name|ent1
operator|->
name|item
operator|->
name|flags
operator|&
name|UNINTERESTING
condition|)
name|swap
operator|=
literal|1
expr_stmt|;
name|sha1
index|[
name|swap
index|]
operator|=
name|ent0
operator|->
name|item
operator|->
name|oid
operator|.
name|hash
expr_stmt|;
name|sha1
index|[
literal|1
operator|-
name|swap
index|]
operator|=
name|ent1
operator|->
name|item
operator|->
name|oid
operator|.
name|hash
expr_stmt|;
name|diff_tree_sha1
argument_list|(
name|sha1
index|[
literal|0
index|]
argument_list|,
name|sha1
index|[
literal|1
index|]
argument_list|,
literal|""
argument_list|,
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|log_tree_diff_flush
argument_list|(
name|revs
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|builtin_diff_combined
specifier|static
name|int
name|builtin_diff_combined
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|struct
name|object_array_entry
modifier|*
name|ent
parameter_list|,
name|int
name|ents
parameter_list|)
block|{
name|struct
name|sha1_array
name|parents
init|=
name|SHA1_ARRAY_INIT
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|revs
operator|->
name|dense_combined_merges
operator|&&
operator|!
name|revs
operator|->
name|combine_merges
condition|)
name|revs
operator|->
name|dense_combined_merges
operator|=
name|revs
operator|->
name|combine_merges
operator|=
literal|1
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|ents
condition|;
name|i
operator|++
control|)
name|sha1_array_append
argument_list|(
operator|&
name|parents
argument_list|,
name|ent
index|[
name|i
index|]
operator|.
name|item
operator|->
name|oid
operator|.
name|hash
argument_list|)
expr_stmt|;
name|diff_tree_combined
argument_list|(
name|ent
index|[
literal|0
index|]
operator|.
name|item
operator|->
name|oid
operator|.
name|hash
argument_list|,
operator|&
name|parents
argument_list|,
name|revs
operator|->
name|dense_combined_merges
argument_list|,
name|revs
argument_list|)
expr_stmt|;
name|sha1_array_clear
argument_list|(
operator|&
name|parents
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|refresh_index_quietly
specifier|static
name|void
name|refresh_index_quietly
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|lock_file
modifier|*
name|lock_file
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|lock_file
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|lock_file
argument_list|)
argument_list|)
expr_stmt|;
name|fd
operator|=
name|hold_locked_index
argument_list|(
name|lock_file
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return;
name|discard_cache
argument_list|()
expr_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
name|refresh_cache
argument_list|(
name|REFRESH_QUIET
operator||
name|REFRESH_UNMERGED
argument_list|)
expr_stmt|;
name|update_index_if_able
argument_list|(
operator|&
name|the_index
argument_list|,
name|lock_file
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|builtin_diff_files
specifier|static
name|int
name|builtin_diff_files
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
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
name|unsigned
name|int
name|options
init|=
literal|0
decl_stmt|;
while|while
condition|(
literal|1
operator|<
name|argc
operator|&&
name|argv
index|[
literal|1
index|]
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--base"
argument_list|)
condition|)
name|revs
operator|->
name|max_count
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--ours"
argument_list|)
condition|)
name|revs
operator|->
name|max_count
operator|=
literal|2
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--theirs"
argument_list|)
condition|)
name|revs
operator|->
name|max_count
operator|=
literal|3
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-q"
argument_list|)
condition|)
name|options
operator||=
name|DIFF_SILENT_ON_REMOVED
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"-h"
argument_list|)
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
else|else
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"invalid option: %s"
argument_list|)
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
return|;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
block|}
comment|/* 	 * "diff --base" should not combine merges because it was not 	 * asked to.  "diff -c" should not densify (if the user wants 	 * dense one, --cc can be explicitly asked for, or just rely 	 * on the default). 	 */
if|if
condition|(
name|revs
operator|->
name|max_count
operator|==
operator|-
literal|1
operator|&&
operator|!
name|revs
operator|->
name|combine_merges
operator|&&
operator|(
name|revs
operator|->
name|diffopt
operator|.
name|output_format
operator|&
name|DIFF_FORMAT_PATCH
operator|)
condition|)
name|revs
operator|->
name|combine_merges
operator|=
name|revs
operator|->
name|dense_combined_merges
operator|=
literal|1
expr_stmt|;
name|setup_work_tree
argument_list|()
expr_stmt|;
if|if
condition|(
name|read_cache_preload
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
operator|.
name|pathspec
argument_list|)
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
literal|"read_cache_preload"
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
return|return
name|run_diff_files
argument_list|(
name|revs
argument_list|,
name|options
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|cmd_diff
name|int
name|cmd_diff
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
name|int
name|i
decl_stmt|;
name|struct
name|rev_info
name|rev
decl_stmt|;
name|struct
name|object_array
name|ent
init|=
name|OBJECT_ARRAY_INIT
decl_stmt|;
name|int
name|blobs
init|=
literal|0
decl_stmt|,
name|paths
init|=
literal|0
decl_stmt|;
name|struct
name|blobinfo
name|blob
index|[
literal|2
index|]
decl_stmt|;
name|int
name|nongit
init|=
literal|0
decl_stmt|,
name|no_index
init|=
literal|0
decl_stmt|;
name|int
name|result
init|=
literal|0
decl_stmt|;
comment|/* 	 * We could get N tree-ish in the rev.pending_objects list. 	 * Also there could be M blobs there, and P pathspecs. 	 * 	 * N=0, M=0: 	 *	cache vs files (diff-files) 	 * N=0, M=2: 	 *      compare two random blobs.  P must be zero. 	 * N=0, M=1, P=1: 	 *	compare a blob with a working tree file. 	 * 	 * N=1, M=0: 	 *      tree vs cache (diff-index --cached) 	 * 	 * N=2, M=0: 	 *      tree vs tree (diff-tree) 	 * 	 * N=0, M=0, P=2: 	 *      compare two filesystem entities (aka --no-index). 	 * 	 * Other cases are errors. 	 */
comment|/* Were we asked to do --no-index explicitly? */
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|i
operator|++
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--no-index"
argument_list|)
condition|)
name|no_index
operator|=
name|DIFF_NO_INDEX_EXPLICIT
expr_stmt|;
if|if
condition|(
name|argv
index|[
name|i
index|]
index|[
literal|0
index|]
operator|!=
literal|'-'
condition|)
break|break;
block|}
if|if
condition|(
operator|!
name|no_index
condition|)
name|prefix
operator|=
name|setup_git_directory_gently
argument_list|(
operator|&
name|nongit
argument_list|)
expr_stmt|;
comment|/* 	 * Treat git diff with at least one path outside of the 	 * repo the same as if the command would have been executed 	 * outside of a git repository.  In this case it behaves 	 * the same way as "git diff --no-index<a><b>", which acts 	 * as a colourful "diff" replacement. 	 */
if|if
condition|(
name|nongit
operator|||
operator|(
operator|(
name|argc
operator|==
name|i
operator|+
literal|2
operator|)
operator|&&
operator|(
operator|!
name|path_inside_repo
argument_list|(
name|prefix
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
operator|||
operator|!
name|path_inside_repo
argument_list|(
name|prefix
argument_list|,
name|argv
index|[
name|i
operator|+
literal|1
index|]
argument_list|)
operator|)
operator|)
condition|)
name|no_index
operator|=
name|DIFF_NO_INDEX_IMPLICIT
expr_stmt|;
if|if
condition|(
operator|!
name|no_index
condition|)
name|gitmodules_config
argument_list|()
expr_stmt|;
name|git_config
argument_list|(
name|git_diff_ui_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|rev
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
if|if
condition|(
name|no_index
operator|&&
name|argc
operator|!=
name|i
operator|+
literal|2
condition|)
block|{
if|if
condition|(
name|no_index
operator|==
name|DIFF_NO_INDEX_IMPLICIT
condition|)
block|{
comment|/* 			 * There was no --no-index and there were not two 			 * paths. It is possible that the user intended 			 * to do an inside-repository operation. 			 */
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Not a git repository\n"
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"To compare two paths outside a working tree:\n"
argument_list|)
expr_stmt|;
block|}
comment|/* Give the usage message for non-repository usage and exit. */
name|usagef
argument_list|(
literal|"git diff %s<path><path>"
argument_list|,
name|no_index
operator|==
name|DIFF_NO_INDEX_EXPLICIT
condition|?
literal|"--no-index"
else|:
literal|"[--no-index]"
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|no_index
condition|)
comment|/* If this is a no-index diff, just run it and exit there. */
name|diff_no_index
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|)
expr_stmt|;
comment|/* Otherwise, we are doing the usual "git" diff */
name|rev
operator|.
name|diffopt
operator|.
name|skip_stat_unmatch
operator|=
operator|!
operator|!
name|diff_auto_refresh_index
expr_stmt|;
comment|/* Scale to real terminal size and respect statGraphWidth config */
name|rev
operator|.
name|diffopt
operator|.
name|stat_width
operator|=
operator|-
literal|1
expr_stmt|;
name|rev
operator|.
name|diffopt
operator|.
name|stat_graph_width
operator|=
operator|-
literal|1
expr_stmt|;
comment|/* Default to let external and textconv be used */
name|DIFF_OPT_SET
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|ALLOW_EXTERNAL
argument_list|)
expr_stmt|;
name|DIFF_OPT_SET
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|ALLOW_TEXTCONV
argument_list|)
expr_stmt|;
if|if
condition|(
name|nongit
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Not a git repository"
argument_list|)
argument_list|)
expr_stmt|;
name|argc
operator|=
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|rev
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|rev
operator|.
name|diffopt
operator|.
name|output_format
condition|)
block|{
name|rev
operator|.
name|diffopt
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_PATCH
expr_stmt|;
name|diff_setup_done
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|)
expr_stmt|;
block|}
name|DIFF_OPT_SET
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|RECURSIVE
argument_list|)
expr_stmt|;
name|setup_diff_pager
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|)
expr_stmt|;
comment|/* 	 * Do we have --cached and not have a pending object, then 	 * default to HEAD by hand.  Eek. 	 */
if|if
condition|(
operator|!
name|rev
operator|.
name|pending
operator|.
name|nr
condition|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--"
argument_list|)
condition|)
break|break;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--cached"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--staged"
argument_list|)
condition|)
block|{
name|add_head_to_pending
argument_list|(
operator|&
name|rev
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|rev
operator|.
name|pending
operator|.
name|nr
condition|)
block|{
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
name|tree
operator|=
name|lookup_tree
argument_list|(
name|EMPTY_TREE_SHA1_BIN
argument_list|)
expr_stmt|;
name|add_pending_object
argument_list|(
operator|&
name|rev
argument_list|,
operator|&
name|tree
operator|->
name|object
argument_list|,
literal|"HEAD"
argument_list|)
expr_stmt|;
block|}
break|break;
block|}
block|}
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|rev
operator|.
name|pending
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object_array_entry
modifier|*
name|entry
init|=
operator|&
name|rev
operator|.
name|pending
operator|.
name|objects
index|[
name|i
index|]
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
init|=
name|entry
operator|->
name|item
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
name|entry
operator|->
name|name
decl_stmt|;
name|int
name|flags
init|=
operator|(
name|obj
operator|->
name|flags
operator|&
name|UNINTERESTING
operator|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
operator|->
name|parsed
condition|)
name|obj
operator|=
name|parse_object
argument_list|(
name|obj
operator|->
name|oid
operator|.
name|hash
argument_list|)
expr_stmt|;
name|obj
operator|=
name|deref_tag
argument_list|(
name|obj
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"invalid object '%s' given."
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_COMMIT
condition|)
name|obj
operator|=
operator|&
operator|(
operator|(
expr|struct
name|commit
operator|*
operator|)
name|obj
operator|)
operator|->
name|tree
operator|->
name|object
expr_stmt|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_TREE
condition|)
block|{
name|obj
operator|->
name|flags
operator||=
name|flags
expr_stmt|;
name|add_object_array
argument_list|(
name|obj
argument_list|,
name|name
argument_list|,
operator|&
name|ent
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_BLOB
condition|)
block|{
if|if
condition|(
literal|2
operator|<=
name|blobs
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"more than two blobs given: '%s'"
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|blob
index|[
name|blobs
index|]
operator|.
name|sha1
argument_list|,
name|obj
operator|->
name|oid
operator|.
name|hash
argument_list|)
expr_stmt|;
name|blob
index|[
name|blobs
index|]
operator|.
name|name
operator|=
name|name
expr_stmt|;
name|blob
index|[
name|blobs
index|]
operator|.
name|mode
operator|=
name|entry
operator|->
name|mode
expr_stmt|;
name|blobs
operator|++
expr_stmt|;
block|}
else|else
block|{
name|die
argument_list|(
name|_
argument_list|(
literal|"unhandled object '%s' given."
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|rev
operator|.
name|prune_data
operator|.
name|nr
condition|)
name|paths
operator|+=
name|rev
operator|.
name|prune_data
operator|.
name|nr
expr_stmt|;
comment|/* 	 * Now, do the arguments look reasonable? 	 */
if|if
condition|(
operator|!
name|ent
operator|.
name|nr
condition|)
block|{
switch|switch
condition|(
name|blobs
condition|)
block|{
case|case
literal|0
case|:
name|result
operator|=
name|builtin_diff_files
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|)
expr_stmt|;
break|break;
case|case
literal|1
case|:
if|if
condition|(
name|paths
operator|!=
literal|1
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
name|result
operator|=
name|builtin_diff_b_f
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|blob
argument_list|)
expr_stmt|;
break|break;
case|case
literal|2
case|:
if|if
condition|(
name|paths
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
name|result
operator|=
name|builtin_diff_blobs
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|blob
argument_list|)
expr_stmt|;
break|break;
default|default:
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|blobs
condition|)
name|usage
argument_list|(
name|builtin_diff_usage
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|ent
operator|.
name|nr
operator|==
literal|1
condition|)
name|result
operator|=
name|builtin_diff_index
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|ent
operator|.
name|nr
operator|==
literal|2
condition|)
name|result
operator|=
name|builtin_diff_tree
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|ent
operator|.
name|objects
index|[
literal|0
index|]
argument_list|,
operator|&
name|ent
operator|.
name|objects
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|ent
operator|.
name|objects
index|[
literal|0
index|]
operator|.
name|item
operator|->
name|flags
operator|&
name|UNINTERESTING
condition|)
block|{
comment|/* 		 * diff A...B where there is at least one merge base 		 * between A and B.  We have ent.objects[0] == 		 * merge-base, ent.objects[ents-2] == A, and 		 * ent.objects[ents-1] == B.  Show diff between the 		 * base and B.  Note that we pick one merge base at 		 * random if there are more than one. 		 */
name|result
operator|=
name|builtin_diff_tree
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|ent
operator|.
name|objects
index|[
literal|0
index|]
argument_list|,
operator|&
name|ent
operator|.
name|objects
index|[
name|ent
operator|.
name|nr
operator|-
literal|1
index|]
argument_list|)
expr_stmt|;
block|}
else|else
name|result
operator|=
name|builtin_diff_combined
argument_list|(
operator|&
name|rev
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|,
name|ent
operator|.
name|objects
argument_list|,
name|ent
operator|.
name|nr
argument_list|)
expr_stmt|;
name|result
operator|=
name|diff_result_code
argument_list|(
operator|&
name|rev
operator|.
name|diffopt
argument_list|,
name|result
argument_list|)
expr_stmt|;
if|if
condition|(
literal|1
operator|<
name|rev
operator|.
name|diffopt
operator|.
name|skip_stat_unmatch
condition|)
name|refresh_index_quietly
argument_list|()
expr_stmt|;
return|return
name|result
return|;
block|}
end_function
end_unit
