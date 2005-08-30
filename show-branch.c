begin_unit
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
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
file|"refs.h"
end_include
begin_decl_stmt
DECL|variable|show_branch_usage
specifier|static
specifier|const
name|char
name|show_branch_usage
index|[]
init|=
literal|"git-show-branch [--all] [--heads] [--tags] [--more=count] [<refs>...]"
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|UNINTERESTING
define|#
directive|define
name|UNINTERESTING
value|01
end_define
begin_define
DECL|macro|REV_SHIFT
define|#
directive|define
name|REV_SHIFT
value|2
end_define
begin_define
DECL|macro|MAX_REVS
define|#
directive|define
name|MAX_REVS
value|29
end_define
begin_comment
DECL|macro|MAX_REVS
comment|/* should not exceed bits_per_int - REV_SHIFT */
end_comment
begin_function
DECL|function|interesting
specifier|static
name|struct
name|commit
modifier|*
name|interesting
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
name|list
operator|=
name|list
operator|->
name|next
expr_stmt|;
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
continue|continue;
return|return
name|commit
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|pop_one_commit
specifier|static
name|struct
name|commit
modifier|*
name|pop_one_commit
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list_p
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|list
decl_stmt|;
name|list
operator|=
operator|*
name|list_p
expr_stmt|;
name|commit
operator|=
name|list
operator|->
name|item
expr_stmt|;
operator|*
name|list_p
operator|=
name|list
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|list
argument_list|)
expr_stmt|;
return|return
name|commit
return|;
block|}
end_function
begin_struct
DECL|struct|commit_name
struct|struct
name|commit_name
block|{
DECL|member|head_name
specifier|const
name|char
modifier|*
name|head_name
decl_stmt|;
comment|/* which head's ancestor? */
DECL|member|generation
name|int
name|generation
decl_stmt|;
comment|/* how many parents away from head_name */
block|}
struct|;
end_struct
begin_comment
comment|/* Name the commit as nth generation ancestor of head_name;  * we count only the first-parent relationship for naming purposes.  */
end_comment
begin_function
DECL|function|name_commit
specifier|static
name|void
name|name_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
specifier|const
name|char
modifier|*
name|head_name
parameter_list|,
name|int
name|nth
parameter_list|)
block|{
name|struct
name|commit_name
modifier|*
name|name
decl_stmt|;
if|if
condition|(
operator|!
name|commit
operator|->
name|object
operator|.
name|util
condition|)
name|commit
operator|->
name|object
operator|.
name|util
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|commit_name
argument_list|)
argument_list|)
expr_stmt|;
name|name
operator|=
name|commit
operator|->
name|object
operator|.
name|util
expr_stmt|;
name|name
operator|->
name|head_name
operator|=
name|head_name
expr_stmt|;
name|name
operator|->
name|generation
operator|=
name|nth
expr_stmt|;
block|}
end_function
begin_comment
comment|/* Parent is the first parent of the commit.  We may name it  * as (n+1)th generation ancestor of the same head_name as  * commit is nth generation ancestore of, if that generation  * number is better than the name it already has.  */
end_comment
begin_function
DECL|function|name_parent
specifier|static
name|void
name|name_parent
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|struct
name|commit
modifier|*
name|parent
parameter_list|)
block|{
name|struct
name|commit_name
modifier|*
name|commit_name
init|=
name|commit
operator|->
name|object
operator|.
name|util
decl_stmt|;
name|struct
name|commit_name
modifier|*
name|parent_name
init|=
name|parent
operator|->
name|object
operator|.
name|util
decl_stmt|;
if|if
condition|(
operator|!
name|commit_name
condition|)
return|return;
if|if
condition|(
operator|!
name|parent_name
operator|||
name|commit_name
operator|->
name|generation
operator|+
literal|1
operator|<
name|parent_name
operator|->
name|generation
condition|)
name|name_commit
argument_list|(
name|parent
argument_list|,
name|commit_name
operator|->
name|head_name
argument_list|,
name|commit_name
operator|->
name|generation
operator|+
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|name_first_parent_chain
specifier|static
name|int
name|name_first_parent_chain
parameter_list|(
name|struct
name|commit
modifier|*
name|c
parameter_list|)
block|{
name|int
name|i
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|c
condition|)
block|{
name|struct
name|commit
modifier|*
name|p
decl_stmt|;
if|if
condition|(
operator|!
name|c
operator|->
name|object
operator|.
name|util
condition|)
break|break;
if|if
condition|(
operator|!
name|c
operator|->
name|parents
condition|)
break|break;
name|p
operator|=
name|c
operator|->
name|parents
operator|->
name|item
expr_stmt|;
if|if
condition|(
operator|!
name|p
operator|->
name|object
operator|.
name|util
condition|)
block|{
name|name_parent
argument_list|(
name|c
argument_list|,
name|p
argument_list|)
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
name|c
operator|=
name|p
expr_stmt|;
block|}
return|return
name|i
return|;
block|}
end_function
begin_function
DECL|function|name_commits
specifier|static
name|void
name|name_commits
parameter_list|(
name|struct
name|commit_list
modifier|*
name|list
parameter_list|,
name|struct
name|commit
modifier|*
modifier|*
name|rev
parameter_list|,
name|char
modifier|*
modifier|*
name|ref_name
parameter_list|,
name|int
name|num_rev
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|cl
decl_stmt|;
name|struct
name|commit
modifier|*
name|c
decl_stmt|;
name|int
name|i
decl_stmt|;
comment|/* First give names to the given heads */
for|for
control|(
name|cl
operator|=
name|list
init|;
name|cl
condition|;
name|cl
operator|=
name|cl
operator|->
name|next
control|)
block|{
name|c
operator|=
name|cl
operator|->
name|item
expr_stmt|;
if|if
condition|(
name|c
operator|->
name|object
operator|.
name|util
condition|)
continue|continue;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_rev
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|rev
index|[
name|i
index|]
operator|==
name|c
condition|)
block|{
name|name_commit
argument_list|(
name|c
argument_list|,
name|ref_name
index|[
name|i
index|]
argument_list|,
literal|0
argument_list|)
expr_stmt|;
break|break;
block|}
block|}
block|}
comment|/* Then commits on the first parent ancestry chain */
do|do
block|{
name|i
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|cl
operator|=
name|list
init|;
name|cl
condition|;
name|cl
operator|=
name|cl
operator|->
name|next
control|)
block|{
name|i
operator|+=
name|name_first_parent_chain
argument_list|(
name|cl
operator|->
name|item
argument_list|)
expr_stmt|;
block|}
block|}
do|while
condition|(
name|i
condition|)
do|;
comment|/* Finally, any unnamed commits */
do|do
block|{
name|i
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|cl
operator|=
name|list
init|;
name|cl
condition|;
name|cl
operator|=
name|cl
operator|->
name|next
control|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
name|struct
name|commit_name
modifier|*
name|n
decl_stmt|;
name|int
name|nth
decl_stmt|;
name|c
operator|=
name|cl
operator|->
name|item
expr_stmt|;
if|if
condition|(
operator|!
name|c
operator|->
name|object
operator|.
name|util
condition|)
continue|continue;
name|n
operator|=
name|c
operator|->
name|object
operator|.
name|util
expr_stmt|;
name|parents
operator|=
name|c
operator|->
name|parents
expr_stmt|;
name|nth
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|commit
modifier|*
name|p
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|char
name|newname
index|[
literal|1000
index|]
decl_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
name|nth
operator|++
expr_stmt|;
if|if
condition|(
name|p
operator|->
name|object
operator|.
name|util
condition|)
continue|continue;
name|sprintf
argument_list|(
name|newname
argument_list|,
literal|"%s^%d"
argument_list|,
name|n
operator|->
name|head_name
argument_list|,
name|nth
argument_list|)
expr_stmt|;
name|name_commit
argument_list|(
name|p
argument_list|,
name|strdup
argument_list|(
name|newname
argument_list|)
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|i
operator|++
expr_stmt|;
name|name_first_parent_chain
argument_list|(
name|p
argument_list|)
expr_stmt|;
block|}
block|}
block|}
do|while
condition|(
name|i
condition|)
do|;
block|}
end_function
begin_function
DECL|function|mark_seen
specifier|static
name|int
name|mark_seen
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|struct
name|commit_list
modifier|*
modifier|*
name|seen_p
parameter_list|)
block|{
if|if
condition|(
operator|!
name|commit
operator|->
name|object
operator|.
name|flags
condition|)
block|{
name|insert_by_date
argument_list|(
name|commit
argument_list|,
name|seen_p
argument_list|)
expr_stmt|;
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
DECL|function|join_revs
specifier|static
name|void
name|join_revs
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list_p
parameter_list|,
name|struct
name|commit_list
modifier|*
modifier|*
name|seen_p
parameter_list|,
name|int
name|num_rev
parameter_list|,
name|int
name|extra
parameter_list|)
block|{
name|int
name|all_mask
init|=
operator|(
operator|(
literal|1u
operator|<<
operator|(
name|REV_SHIFT
operator|+
name|num_rev
operator|)
operator|)
operator|-
literal|1
operator|)
decl_stmt|;
name|int
name|all_revs
init|=
name|all_mask
operator|&
operator|~
operator|(
operator|(
literal|1u
operator|<<
name|REV_SHIFT
operator|)
operator|-
literal|1
operator|)
decl_stmt|;
while|while
condition|(
operator|*
name|list_p
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
init|=
name|pop_one_commit
argument_list|(
name|list_p
argument_list|)
decl_stmt|;
name|int
name|flags
init|=
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|all_mask
decl_stmt|;
name|int
name|still_interesting
init|=
operator|!
operator|!
name|interesting
argument_list|(
operator|*
name|list_p
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|still_interesting
operator|&&
name|extra
operator|<
literal|0
condition|)
break|break;
name|mark_seen
argument_list|(
name|commit
argument_list|,
name|seen_p
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|flags
operator|&
name|all_revs
operator|)
operator|==
name|all_revs
condition|)
name|flags
operator||=
name|UNINTERESTING
expr_stmt|;
name|parents
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|commit
modifier|*
name|p
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|int
name|this_flag
init|=
name|p
operator|->
name|object
operator|.
name|flags
decl_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
if|if
condition|(
operator|(
name|this_flag
operator|&
name|flags
operator|)
operator|==
name|flags
condition|)
continue|continue;
name|parse_commit
argument_list|(
name|p
argument_list|)
expr_stmt|;
if|if
condition|(
name|mark_seen
argument_list|(
name|p
argument_list|,
name|seen_p
argument_list|)
operator|&&
operator|!
name|still_interesting
condition|)
name|extra
operator|--
expr_stmt|;
name|p
operator|->
name|object
operator|.
name|flags
operator||=
name|flags
expr_stmt|;
name|insert_by_date
argument_list|(
name|p
argument_list|,
name|list_p
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|show_one_commit
specifier|static
name|void
name|show_one_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|char
name|pretty
index|[
literal|128
index|]
decl_stmt|,
modifier|*
name|cp
decl_stmt|;
name|struct
name|commit_name
modifier|*
name|name
init|=
name|commit
operator|->
name|object
operator|.
name|util
decl_stmt|;
name|pretty_print_commit
argument_list|(
name|CMIT_FMT_ONELINE
argument_list|,
name|commit
operator|->
name|buffer
argument_list|,
operator|~
literal|0
argument_list|,
name|pretty
argument_list|,
sizeof|sizeof
argument_list|(
name|pretty
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|pretty
argument_list|,
literal|"[PATCH] "
argument_list|,
literal|8
argument_list|)
condition|)
name|cp
operator|=
name|pretty
operator|+
literal|8
expr_stmt|;
else|else
name|cp
operator|=
name|pretty
expr_stmt|;
if|if
condition|(
name|name
operator|&&
name|name
operator|->
name|head_name
condition|)
block|{
name|printf
argument_list|(
literal|"[%s"
argument_list|,
name|name
operator|->
name|head_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|name
operator|->
name|generation
condition|)
name|printf
argument_list|(
literal|"~%d"
argument_list|,
name|name
operator|->
name|generation
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"] "
argument_list|)
expr_stmt|;
block|}
name|puts
argument_list|(
name|cp
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|ref_name
specifier|static
name|char
modifier|*
name|ref_name
index|[
name|MAX_REVS
operator|+
literal|1
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|ref_name_cnt
specifier|static
name|int
name|ref_name_cnt
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|compare_ref_name
specifier|static
name|int
name|compare_ref_name
parameter_list|(
specifier|const
name|void
modifier|*
name|a_
parameter_list|,
specifier|const
name|void
modifier|*
name|b_
parameter_list|)
block|{
specifier|const
name|char
modifier|*
specifier|const
modifier|*
name|a
init|=
name|a_
decl_stmt|,
modifier|*
decl_stmt|const
modifier|*
name|b
init|=
name|b_
decl_stmt|;
return|return
name|strcmp
argument_list|(
operator|*
name|a
argument_list|,
operator|*
name|b
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|sort_ref_range
specifier|static
name|void
name|sort_ref_range
parameter_list|(
name|int
name|bottom
parameter_list|,
name|int
name|top
parameter_list|)
block|{
name|qsort
argument_list|(
name|ref_name
operator|+
name|bottom
argument_list|,
name|top
operator|-
name|bottom
argument_list|,
sizeof|sizeof
argument_list|(
name|ref_name
index|[
literal|0
index|]
argument_list|)
argument_list|,
name|compare_ref_name
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|append_ref
specifier|static
name|int
name|append_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|lookup_commit_reference_gently
argument_list|(
name|sha1
argument_list|,
literal|1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|MAX_REVS
operator|<
name|ref_name_cnt
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"warning: ignoring %s; "
literal|"cannot handle more than %d refs"
argument_list|,
name|refname
argument_list|,
name|MAX_REVS
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|ref_name
index|[
name|ref_name_cnt
operator|++
index|]
operator|=
name|strdup
argument_list|(
name|refname
argument_list|)
expr_stmt|;
name|ref_name
index|[
name|ref_name_cnt
index|]
operator|=
name|NULL
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|append_head_ref
specifier|static
name|int
name|append_head_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
name|strncmp
argument_list|(
name|refname
argument_list|,
literal|"refs/heads/"
argument_list|,
literal|11
argument_list|)
condition|)
return|return
literal|0
return|;
return|return
name|append_ref
argument_list|(
name|refname
operator|+
literal|11
argument_list|,
name|sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|append_tag_ref
specifier|static
name|int
name|append_tag_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
name|strncmp
argument_list|(
name|refname
argument_list|,
literal|"refs/tags/"
argument_list|,
literal|10
argument_list|)
condition|)
return|return
literal|0
return|;
return|return
name|append_ref
argument_list|(
name|refname
operator|+
literal|5
argument_list|,
name|sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|snarf_refs
specifier|static
name|void
name|snarf_refs
parameter_list|(
name|int
name|head
parameter_list|,
name|int
name|tag
parameter_list|)
block|{
if|if
condition|(
name|head
condition|)
block|{
name|int
name|orig_cnt
init|=
name|ref_name_cnt
decl_stmt|;
name|for_each_ref
argument_list|(
name|append_head_ref
argument_list|)
expr_stmt|;
name|sort_ref_range
argument_list|(
name|orig_cnt
argument_list|,
name|ref_name_cnt
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|tag
condition|)
block|{
name|int
name|orig_cnt
init|=
name|ref_name_cnt
decl_stmt|;
name|for_each_ref
argument_list|(
name|append_tag_ref
argument_list|)
expr_stmt|;
name|sort_ref_range
argument_list|(
name|orig_cnt
argument_list|,
name|ref_name_cnt
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|rev_is_head
specifier|static
name|int
name|rev_is_head
parameter_list|(
name|char
modifier|*
name|head_path
parameter_list|,
name|int
name|headlen
parameter_list|,
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|head_sha1
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|namelen
decl_stmt|;
if|if
condition|(
operator|(
operator|!
name|head_path
index|[
literal|0
index|]
operator|)
operator|||
name|memcmp
argument_list|(
name|head_sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
condition|)
return|return
literal|0
return|;
name|namelen
operator|=
name|strlen
argument_list|(
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|headlen
operator|<
name|namelen
operator|)
operator|||
name|memcmp
argument_list|(
name|head_path
operator|+
name|headlen
operator|-
name|namelen
argument_list|,
name|name
argument_list|,
name|namelen
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|headlen
operator|==
name|namelen
operator|||
name|head_path
index|[
name|headlen
operator|-
name|namelen
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|show_merge_base
specifier|static
name|int
name|show_merge_base
parameter_list|(
name|struct
name|commit_list
modifier|*
name|seen
parameter_list|,
name|int
name|num_rev
parameter_list|)
block|{
name|int
name|all_mask
init|=
operator|(
operator|(
literal|1u
operator|<<
operator|(
name|REV_SHIFT
operator|+
name|num_rev
operator|)
operator|)
operator|-
literal|1
operator|)
decl_stmt|;
name|int
name|all_revs
init|=
name|all_mask
operator|&
operator|~
operator|(
operator|(
literal|1u
operator|<<
name|REV_SHIFT
operator|)
operator|-
literal|1
operator|)
decl_stmt|;
while|while
condition|(
name|seen
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|pop_one_commit
argument_list|(
operator|&
name|seen
argument_list|)
decl_stmt|;
name|int
name|flags
init|=
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|all_mask
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|flags
operator|&
name|UNINTERESTING
operator|)
operator|&&
operator|(
operator|(
name|flags
operator|&
name|all_revs
operator|)
operator|==
name|all_revs
operator|)
condition|)
block|{
name|puts
argument_list|(
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|ac
parameter_list|,
name|char
modifier|*
modifier|*
name|av
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|rev
index|[
name|MAX_REVS
index|]
decl_stmt|,
modifier|*
name|commit
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|list
init|=
name|NULL
decl_stmt|,
modifier|*
name|seen
init|=
name|NULL
decl_stmt|;
name|int
name|num_rev
decl_stmt|,
name|i
decl_stmt|,
name|extra
init|=
literal|0
decl_stmt|;
name|int
name|all_heads
init|=
literal|0
decl_stmt|,
name|all_tags
init|=
literal|0
decl_stmt|;
name|int
name|all_mask
decl_stmt|,
name|all_revs
decl_stmt|,
name|shown_merge_point
decl_stmt|;
name|char
name|head_path
index|[
literal|128
index|]
decl_stmt|;
name|int
name|head_path_len
decl_stmt|;
name|unsigned
name|char
name|head_sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|merge_base
init|=
literal|0
decl_stmt|;
name|char
modifier|*
modifier|*
name|label
decl_stmt|;
while|while
condition|(
literal|1
operator|<
name|ac
operator|&&
name|av
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
name|char
modifier|*
name|arg
init|=
name|av
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
literal|"--all"
argument_list|)
condition|)
name|all_heads
operator|=
name|all_tags
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--heads"
argument_list|)
condition|)
name|all_heads
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--tags"
argument_list|)
condition|)
name|all_tags
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--more"
argument_list|)
condition|)
name|extra
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--more="
argument_list|,
literal|7
argument_list|)
condition|)
block|{
name|extra
operator|=
name|atoi
argument_list|(
name|arg
operator|+
literal|7
argument_list|)
expr_stmt|;
if|if
condition|(
name|extra
operator|<
literal|0
condition|)
name|usage
argument_list|(
name|show_branch_usage
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--merge-base"
argument_list|)
condition|)
name|merge_base
operator|=
literal|1
expr_stmt|;
else|else
name|usage
argument_list|(
name|show_branch_usage
argument_list|)
expr_stmt|;
name|ac
operator|--
expr_stmt|;
name|av
operator|++
expr_stmt|;
block|}
name|ac
operator|--
expr_stmt|;
name|av
operator|++
expr_stmt|;
if|if
condition|(
name|all_heads
operator|+
name|all_tags
condition|)
name|snarf_refs
argument_list|(
name|all_heads
argument_list|,
name|all_tags
argument_list|)
expr_stmt|;
while|while
condition|(
literal|0
operator|<
name|ac
condition|)
block|{
name|unsigned
name|char
name|revkey
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
operator|*
name|av
argument_list|,
name|revkey
argument_list|)
condition|)
name|die
argument_list|(
literal|"bad sha1 reference %s"
argument_list|,
operator|*
name|av
argument_list|)
expr_stmt|;
name|append_ref
argument_list|(
operator|*
name|av
argument_list|,
name|revkey
argument_list|)
expr_stmt|;
name|ac
operator|--
expr_stmt|;
name|av
operator|++
expr_stmt|;
block|}
comment|/* If still no revs, then add heads */
if|if
condition|(
operator|!
name|ref_name_cnt
condition|)
name|snarf_refs
argument_list|(
literal|1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
for|for
control|(
name|num_rev
operator|=
literal|0
init|;
name|ref_name
index|[
name|num_rev
index|]
condition|;
name|num_rev
operator|++
control|)
block|{
name|unsigned
name|char
name|revkey
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|MAX_REVS
operator|<=
name|num_rev
condition|)
name|die
argument_list|(
literal|"cannot handle more than %d revs."
argument_list|,
name|MAX_REVS
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|ref_name
index|[
name|num_rev
index|]
argument_list|,
name|revkey
argument_list|)
condition|)
name|usage
argument_list|(
name|show_branch_usage
argument_list|)
expr_stmt|;
name|commit
operator|=
name|lookup_commit_reference
argument_list|(
name|revkey
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
name|die
argument_list|(
literal|"cannot find commit %s (%s)"
argument_list|,
name|ref_name
index|[
name|num_rev
index|]
argument_list|,
name|revkey
argument_list|)
expr_stmt|;
name|parse_commit
argument_list|(
name|commit
argument_list|)
expr_stmt|;
name|mark_seen
argument_list|(
name|commit
argument_list|,
operator|&
name|seen
argument_list|)
expr_stmt|;
comment|/* rev#0 uses bit REV_SHIFT, rev#1 uses bit REV_SHIFT+1, 		 * and so on.  REV_SHIFT bits from bit 0 are used for 		 * internal bookkeeping. 		 */
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
literal|1u
operator|<<
operator|(
name|num_rev
operator|+
name|REV_SHIFT
operator|)
expr_stmt|;
name|insert_by_date
argument_list|(
name|commit
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
name|rev
index|[
name|num_rev
index|]
operator|=
name|commit
expr_stmt|;
block|}
name|join_revs
argument_list|(
operator|&
name|list
argument_list|,
operator|&
name|seen
argument_list|,
name|num_rev
argument_list|,
name|extra
argument_list|)
expr_stmt|;
name|head_path_len
operator|=
name|readlink
argument_list|(
literal|".git/HEAD"
argument_list|,
name|head_path
argument_list|,
sizeof|sizeof
argument_list|(
name|head_path
argument_list|)
operator|-
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|head_path_len
operator|<
literal|0
operator|)
operator|||
name|get_sha1
argument_list|(
literal|"HEAD"
argument_list|,
name|head_sha1
argument_list|)
condition|)
name|head_path
index|[
literal|0
index|]
operator|=
literal|0
expr_stmt|;
else|else
name|head_path
index|[
name|head_path_len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|merge_base
condition|)
return|return
name|show_merge_base
argument_list|(
name|seen
argument_list|,
name|num_rev
argument_list|)
return|;
comment|/* Show list */
if|if
condition|(
literal|1
operator|<
name|num_rev
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_rev
condition|;
name|i
operator|++
control|)
block|{
name|int
name|j
decl_stmt|;
name|int
name|is_head
init|=
name|rev_is_head
argument_list|(
name|head_path
argument_list|,
name|head_path_len
argument_list|,
name|ref_name
index|[
name|i
index|]
argument_list|,
name|head_sha1
argument_list|,
name|rev
index|[
name|i
index|]
operator|->
name|object
operator|.
name|sha1
argument_list|)
decl_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|i
condition|;
name|j
operator|++
control|)
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%c [%s] "
argument_list|,
name|is_head
condition|?
literal|'*'
else|:
literal|'!'
argument_list|,
name|ref_name
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|show_one_commit
argument_list|(
name|rev
index|[
name|i
index|]
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
name|num_rev
condition|;
name|i
operator|++
control|)
name|putchar
argument_list|(
literal|'-'
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
comment|/* Sort topologically */
name|sort_in_topological_order
argument_list|(
operator|&
name|seen
argument_list|)
expr_stmt|;
comment|/* Give names to commits */
name|name_commits
argument_list|(
name|seen
argument_list|,
name|rev
argument_list|,
name|ref_name
argument_list|,
name|num_rev
argument_list|)
expr_stmt|;
name|all_mask
operator|=
operator|(
operator|(
literal|1u
operator|<<
operator|(
name|REV_SHIFT
operator|+
name|num_rev
operator|)
operator|)
operator|-
literal|1
operator|)
expr_stmt|;
name|all_revs
operator|=
name|all_mask
operator|&
operator|~
operator|(
operator|(
literal|1u
operator|<<
name|REV_SHIFT
operator|)
operator|-
literal|1
operator|)
expr_stmt|;
name|shown_merge_point
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|seen
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|pop_one_commit
argument_list|(
operator|&
name|seen
argument_list|)
decl_stmt|;
name|int
name|this_flag
init|=
name|commit
operator|->
name|object
operator|.
name|flags
decl_stmt|;
name|int
name|is_merge_point
init|=
operator|(
name|this_flag
operator|&
name|all_revs
operator|)
operator|==
name|all_revs
decl_stmt|;
specifier|static
name|char
modifier|*
name|obvious
index|[]
init|=
block|{
literal|""
block|}
decl_stmt|;
if|if
condition|(
name|is_merge_point
condition|)
name|shown_merge_point
operator|=
literal|1
expr_stmt|;
if|if
condition|(
literal|1
operator|<
name|num_rev
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_rev
condition|;
name|i
operator|++
control|)
name|putchar
argument_list|(
operator|(
name|this_flag
operator|&
operator|(
literal|1u
operator|<<
operator|(
name|i
operator|+
name|REV_SHIFT
operator|)
operator|)
operator|)
condition|?
literal|'+'
else|:
literal|' '
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
block|}
name|show_one_commit
argument_list|(
name|commit
argument_list|)
expr_stmt|;
if|if
condition|(
name|num_rev
operator|==
literal|1
condition|)
name|label
operator|=
name|obvious
expr_stmt|;
if|if
condition|(
name|shown_merge_point
operator|&&
name|is_merge_point
condition|)
if|if
condition|(
operator|--
name|extra
operator|<
literal|0
condition|)
break|break;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
