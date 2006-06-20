begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
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
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_comment
comment|/* bits #0-15 in revision.h */
end_comment
begin_define
DECL|macro|COUNTED
define|#
directive|define
name|COUNTED
value|(1u<<16)
end_define
begin_decl_stmt
DECL|variable|rev_list_usage
specifier|static
specifier|const
name|char
name|rev_list_usage
index|[]
init|=
literal|"git-rev-list [OPTION]<commit-id>... [ -- paths... ]\n"
literal|"  limiting output:\n"
literal|"    --max-count=nr\n"
literal|"    --max-age=epoch\n"
literal|"    --min-age=epoch\n"
literal|"    --sparse\n"
literal|"    --no-merges\n"
literal|"    --remove-empty\n"
literal|"    --all\n"
literal|"  ordering output:\n"
literal|"    --topo-order\n"
literal|"    --date-order\n"
literal|"  formatting output:\n"
literal|"    --parents\n"
literal|"    --objects | --objects-edge\n"
literal|"    --unpacked\n"
literal|"    --header | --pretty\n"
literal|"    --abbrev=nr | --no-abbrev\n"
literal|"    --abbrev-commit\n"
literal|"  special purpose:\n"
literal|"    --bisect"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|revs
specifier|static
name|struct
name|rev_info
name|revs
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|bisect_list
specifier|static
name|int
name|bisect_list
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|show_timestamp
specifier|static
name|int
name|show_timestamp
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|hdr_termination
specifier|static
name|int
name|hdr_termination
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|header_prefix
specifier|static
specifier|const
name|char
modifier|*
name|header_prefix
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|show_commit
specifier|static
name|void
name|show_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
if|if
condition|(
name|show_timestamp
condition|)
name|printf
argument_list|(
literal|"%lu "
argument_list|,
name|commit
operator|->
name|date
argument_list|)
expr_stmt|;
if|if
condition|(
name|header_prefix
condition|)
name|fputs
argument_list|(
name|header_prefix
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|BOUNDARY
condition|)
name|putchar
argument_list|(
literal|'-'
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|.
name|abbrev_commit
operator|&&
name|revs
operator|.
name|abbrev
condition|)
name|fputs
argument_list|(
name|find_unique_abbrev
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|revs
operator|.
name|abbrev
argument_list|)
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
else|else
name|fputs
argument_list|(
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|.
name|parents
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|commit
operator|->
name|parents
decl_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|object
modifier|*
name|o
init|=
operator|&
operator|(
name|parents
operator|->
name|item
operator|->
name|object
operator|)
decl_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|o
operator|->
name|flags
operator|&
name|TMP_MARK
condition|)
continue|continue;
name|printf
argument_list|(
literal|" %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|o
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|o
operator|->
name|flags
operator||=
name|TMP_MARK
expr_stmt|;
block|}
comment|/* TMP_MARK is a general purpose flag that can 		 * be used locally, but the user should clean 		 * things up after it is done with them. 		 */
for|for
control|(
name|parents
operator|=
name|commit
operator|->
name|parents
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
control|)
name|parents
operator|->
name|item
operator|->
name|object
operator|.
name|flags
operator|&=
operator|~
name|TMP_MARK
expr_stmt|;
block|}
if|if
condition|(
name|revs
operator|.
name|commit_format
operator|==
name|CMIT_FMT_ONELINE
condition|)
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
else|else
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|.
name|verbose_header
condition|)
block|{
specifier|static
name|char
name|pretty_header
index|[
literal|16384
index|]
decl_stmt|;
name|pretty_print_commit
argument_list|(
name|revs
operator|.
name|commit_format
argument_list|,
name|commit
argument_list|,
operator|~
literal|0
argument_list|,
name|pretty_header
argument_list|,
sizeof|sizeof
argument_list|(
name|pretty_header
argument_list|)
argument_list|,
name|revs
operator|.
name|abbrev
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|pretty_header
argument_list|,
name|hdr_termination
argument_list|)
expr_stmt|;
block|}
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|parents
condition|)
block|{
name|free_commit_list
argument_list|(
name|commit
operator|->
name|parents
argument_list|)
expr_stmt|;
name|commit
operator|->
name|parents
operator|=
name|NULL
expr_stmt|;
block|}
if|if
condition|(
name|commit
operator|->
name|buffer
condition|)
block|{
name|free
argument_list|(
name|commit
operator|->
name|buffer
argument_list|)
expr_stmt|;
name|commit
operator|->
name|buffer
operator|=
name|NULL
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|process_blob
specifier|static
name|void
name|process_blob
parameter_list|(
name|struct
name|blob
modifier|*
name|blob
parameter_list|,
name|struct
name|object_array
modifier|*
name|p
parameter_list|,
name|struct
name|name_path
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
operator|&
name|blob
operator|->
name|object
decl_stmt|;
if|if
condition|(
operator|!
name|revs
operator|.
name|blob_objects
condition|)
return|return;
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
operator|(
name|UNINTERESTING
operator||
name|SEEN
operator|)
condition|)
return|return;
name|obj
operator|->
name|flags
operator||=
name|SEEN
expr_stmt|;
name|name
operator|=
name|strdup
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|add_object
argument_list|(
name|obj
argument_list|,
name|p
argument_list|,
name|path
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|process_tree
specifier|static
name|void
name|process_tree
parameter_list|(
name|struct
name|tree
modifier|*
name|tree
parameter_list|,
name|struct
name|object_array
modifier|*
name|p
parameter_list|,
name|struct
name|name_path
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
operator|&
name|tree
operator|->
name|object
decl_stmt|;
name|struct
name|tree_desc
name|desc
decl_stmt|;
name|struct
name|name_entry
name|entry
decl_stmt|;
name|struct
name|name_path
name|me
decl_stmt|;
if|if
condition|(
operator|!
name|revs
operator|.
name|tree_objects
condition|)
return|return;
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
operator|(
name|UNINTERESTING
operator||
name|SEEN
operator|)
condition|)
return|return;
if|if
condition|(
name|parse_tree
argument_list|(
name|tree
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"bad tree object %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|obj
operator|->
name|flags
operator||=
name|SEEN
expr_stmt|;
name|name
operator|=
name|strdup
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|add_object
argument_list|(
name|obj
argument_list|,
name|p
argument_list|,
name|path
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|me
operator|.
name|up
operator|=
name|path
expr_stmt|;
name|me
operator|.
name|elem
operator|=
name|name
expr_stmt|;
name|me
operator|.
name|elem_len
operator|=
name|strlen
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|desc
operator|.
name|buf
operator|=
name|tree
operator|->
name|buffer
expr_stmt|;
name|desc
operator|.
name|size
operator|=
name|tree
operator|->
name|size
expr_stmt|;
while|while
condition|(
name|tree_entry
argument_list|(
operator|&
name|desc
argument_list|,
operator|&
name|entry
argument_list|)
condition|)
block|{
if|if
condition|(
name|S_ISDIR
argument_list|(
name|entry
operator|.
name|mode
argument_list|)
condition|)
name|process_tree
argument_list|(
name|lookup_tree
argument_list|(
name|entry
operator|.
name|sha1
argument_list|)
argument_list|,
name|p
argument_list|,
operator|&
name|me
argument_list|,
name|entry
operator|.
name|path
argument_list|)
expr_stmt|;
else|else
name|process_blob
argument_list|(
name|lookup_blob
argument_list|(
name|entry
operator|.
name|sha1
argument_list|)
argument_list|,
name|p
argument_list|,
operator|&
name|me
argument_list|,
name|entry
operator|.
name|path
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|tree
operator|->
name|buffer
argument_list|)
expr_stmt|;
name|tree
operator|->
name|buffer
operator|=
name|NULL
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_commit_list
specifier|static
name|void
name|show_commit_list
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|struct
name|object_array
name|objects
init|=
block|{
literal|0
block|,
literal|0
block|,
name|NULL
block|}
decl_stmt|;
while|while
condition|(
operator|(
name|commit
operator|=
name|get_revision
argument_list|(
name|revs
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|process_tree
argument_list|(
name|commit
operator|->
name|tree
argument_list|,
operator|&
name|objects
argument_list|,
name|NULL
argument_list|,
literal|""
argument_list|)
expr_stmt|;
name|show_commit
argument_list|(
name|commit
argument_list|)
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|revs
operator|->
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
name|pending
init|=
name|revs
operator|->
name|pending
operator|.
name|objects
operator|+
name|i
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
init|=
name|pending
operator|->
name|item
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
name|pending
operator|->
name|name
decl_stmt|;
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
operator|(
name|UNINTERESTING
operator||
name|SEEN
operator|)
condition|)
continue|continue;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|TYPE_TAG
condition|)
block|{
name|obj
operator|->
name|flags
operator||=
name|SEEN
expr_stmt|;
name|add_object_array
argument_list|(
name|obj
argument_list|,
name|name
argument_list|,
operator|&
name|objects
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|TYPE_TREE
condition|)
block|{
name|process_tree
argument_list|(
operator|(
expr|struct
name|tree
operator|*
operator|)
name|obj
argument_list|,
operator|&
name|objects
argument_list|,
name|NULL
argument_list|,
name|name
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|TYPE_BLOB
condition|)
block|{
name|process_blob
argument_list|(
operator|(
expr|struct
name|blob
operator|*
operator|)
name|obj
argument_list|,
operator|&
name|objects
argument_list|,
name|NULL
argument_list|,
name|name
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|die
argument_list|(
literal|"unknown pending object %s (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|objects
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
name|p
init|=
name|objects
operator|.
name|objects
operator|+
name|i
decl_stmt|;
comment|/* An object with name "foo\n0000000..." can be used to 		 * confuse downstream git-pack-objects very badly. 		 */
specifier|const
name|char
modifier|*
name|ep
init|=
name|strchr
argument_list|(
name|p
operator|->
name|name
argument_list|,
literal|'\n'
argument_list|)
decl_stmt|;
if|if
condition|(
name|ep
condition|)
block|{
name|printf
argument_list|(
literal|"%s %.*s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|p
operator|->
name|item
operator|->
name|sha1
argument_list|)
argument_list|,
call|(
name|int
call|)
argument_list|(
name|ep
operator|-
name|p
operator|->
name|name
argument_list|)
argument_list|,
name|p
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
else|else
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|p
operator|->
name|item
operator|->
name|sha1
argument_list|)
argument_list|,
name|p
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * This is a truly stupid algorithm, but it's only  * used for bisection, and we just don't care enough.  *  * We care just barely enough to avoid recursing for  * non-merge entries.  */
end_comment
begin_function
DECL|function|count_distance
specifier|static
name|int
name|count_distance
parameter_list|(
name|struct
name|commit_list
modifier|*
name|entry
parameter_list|)
block|{
name|int
name|nr
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|entry
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|entry
operator|->
name|item
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
operator|(
name|UNINTERESTING
operator||
name|COUNTED
operator|)
condition|)
break|break;
if|if
condition|(
operator|!
name|revs
operator|.
name|prune_fn
operator|||
operator|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|TREECHANGE
operator|)
condition|)
name|nr
operator|++
expr_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|COUNTED
expr_stmt|;
name|p
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
name|entry
operator|=
name|p
expr_stmt|;
if|if
condition|(
name|p
condition|)
block|{
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
name|nr
operator|+=
name|count_distance
argument_list|(
name|p
argument_list|)
expr_stmt|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
block|}
block|}
return|return
name|nr
return|;
block|}
end_function
begin_function
DECL|function|clear_distance
specifier|static
name|void
name|clear_distance
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|list
operator|->
name|item
decl_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator|&=
operator|~
name|COUNTED
expr_stmt|;
name|list
operator|=
name|list
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|find_bisection
specifier|static
name|struct
name|commit_list
modifier|*
name|find_bisection
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
name|int
name|nr
decl_stmt|,
name|closest
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|,
modifier|*
name|best
decl_stmt|;
name|nr
operator|=
literal|0
expr_stmt|;
name|p
operator|=
name|list
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
if|if
condition|(
operator|!
name|revs
operator|.
name|prune_fn
operator|||
operator|(
name|p
operator|->
name|item
operator|->
name|object
operator|.
name|flags
operator|&
name|TREECHANGE
operator|)
condition|)
name|nr
operator|++
expr_stmt|;
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
name|closest
operator|=
literal|0
expr_stmt|;
name|best
operator|=
name|list
expr_stmt|;
for|for
control|(
name|p
operator|=
name|list
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
name|int
name|distance
decl_stmt|;
if|if
condition|(
name|revs
operator|.
name|prune_fn
operator|&&
operator|!
operator|(
name|p
operator|->
name|item
operator|->
name|object
operator|.
name|flags
operator|&
name|TREECHANGE
operator|)
condition|)
continue|continue;
name|distance
operator|=
name|count_distance
argument_list|(
name|p
argument_list|)
expr_stmt|;
name|clear_distance
argument_list|(
name|list
argument_list|)
expr_stmt|;
if|if
condition|(
name|nr
operator|-
name|distance
operator|<
name|distance
condition|)
name|distance
operator|=
name|nr
operator|-
name|distance
expr_stmt|;
if|if
condition|(
name|distance
operator|>
name|closest
condition|)
block|{
name|best
operator|=
name|p
expr_stmt|;
name|closest
operator|=
name|distance
expr_stmt|;
block|}
block|}
if|if
condition|(
name|best
condition|)
name|best
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
return|return
name|best
return|;
block|}
end_function
begin_function
DECL|function|mark_edge_parents_uninteresting
specifier|static
name|void
name|mark_edge_parents_uninteresting
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
for|for
control|(
name|parents
operator|=
name|commit
operator|->
name|parents
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
control|)
block|{
name|struct
name|commit
modifier|*
name|parent
init|=
name|parents
operator|->
name|item
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|parent
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
operator|)
condition|)
continue|continue;
name|mark_tree_uninteresting
argument_list|(
name|parent
operator|->
name|tree
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|.
name|edge_hint
operator|&&
operator|!
operator|(
name|parent
operator|->
name|object
operator|.
name|flags
operator|&
name|SHOWN
operator|)
condition|)
block|{
name|parent
operator|->
name|object
operator|.
name|flags
operator||=
name|SHOWN
expr_stmt|;
name|printf
argument_list|(
literal|"-%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|parent
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|mark_edges_uninteresting
specifier|static
name|void
name|mark_edges_uninteresting
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|)
block|{
for|for
control|(
init|;
name|list
condition|;
name|list
operator|=
name|list
operator|->
name|next
control|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|list
operator|->
name|item
decl_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|UNINTERESTING
condition|)
block|{
name|mark_tree_uninteresting
argument_list|(
name|commit
operator|->
name|tree
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|mark_edge_parents_uninteresting
argument_list|(
name|commit
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|cmd_rev_list
name|int
name|cmd_rev_list
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
name|char
modifier|*
modifier|*
name|envp
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|list
decl_stmt|;
name|int
name|i
decl_stmt|;
name|init_revisions
argument_list|(
operator|&
name|revs
argument_list|)
expr_stmt|;
name|revs
operator|.
name|abbrev
operator|=
literal|0
expr_stmt|;
name|revs
operator|.
name|commit_format
operator|=
name|CMIT_FMT_UNSPECIFIED
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
name|revs
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
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
literal|"--header"
argument_list|)
condition|)
block|{
name|revs
operator|.
name|verbose_header
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--timestamp"
argument_list|)
condition|)
block|{
name|show_timestamp
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--bisect"
argument_list|)
condition|)
block|{
name|bisect_list
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|rev_list_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|revs
operator|.
name|commit_format
operator|!=
name|CMIT_FMT_UNSPECIFIED
condition|)
block|{
comment|/* The command line has a --pretty  */
name|hdr_termination
operator|=
literal|'\n'
expr_stmt|;
if|if
condition|(
name|revs
operator|.
name|commit_format
operator|==
name|CMIT_FMT_ONELINE
condition|)
name|header_prefix
operator|=
literal|""
expr_stmt|;
else|else
name|header_prefix
operator|=
literal|"commit "
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|revs
operator|.
name|verbose_header
condition|)
comment|/* Only --header was specified */
name|revs
operator|.
name|commit_format
operator|=
name|CMIT_FMT_RAW
expr_stmt|;
name|list
operator|=
name|revs
operator|.
name|commits
expr_stmt|;
if|if
condition|(
operator|(
operator|!
name|list
operator|&&
operator|(
operator|!
operator|(
name|revs
operator|.
name|tag_objects
operator|||
name|revs
operator|.
name|tree_objects
operator|||
name|revs
operator|.
name|blob_objects
operator|)
operator|&&
operator|!
name|revs
operator|.
name|pending
operator|.
name|nr
operator|)
operator|)
operator|||
name|revs
operator|.
name|diff
condition|)
name|usage
argument_list|(
name|rev_list_usage
argument_list|)
expr_stmt|;
name|save_commit_buffer
operator|=
name|revs
operator|.
name|verbose_header
expr_stmt|;
name|track_object_refs
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|bisect_list
condition|)
name|revs
operator|.
name|limited
operator|=
literal|1
expr_stmt|;
name|prepare_revision_walk
argument_list|(
operator|&
name|revs
argument_list|)
expr_stmt|;
if|if
condition|(
name|revs
operator|.
name|tree_objects
condition|)
name|mark_edges_uninteresting
argument_list|(
name|revs
operator|.
name|commits
argument_list|)
expr_stmt|;
if|if
condition|(
name|bisect_list
condition|)
name|revs
operator|.
name|commits
operator|=
name|find_bisection
argument_list|(
name|revs
operator|.
name|commits
argument_list|)
expr_stmt|;
name|show_commit_list
argument_list|(
operator|&
name|revs
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
