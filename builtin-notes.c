begin_unit
begin_comment
comment|/*  * Builtin "git notes"  *  * Copyright (c) 2010 Johan Herland<johan@herland.net>  *  * Based on git-notes.sh by Johannes Schindelin,  * and builtin-tag.c by Kristian HÃ¸gsberg and Carlos Rica.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"notes.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
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
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|git_notes_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|git_notes_usage
index|[]
init|=
block|{
literal|"git notes [list [<object>]]"
block|,
literal|"git notes add [-f] [-m<msg> | -F<file>] [<object>]"
block|,
literal|"git notes edit [-m<msg> | -F<file>] [<object>]"
block|,
literal|"git notes show [<object>]"
block|,
literal|"git notes remove [<object>]"
block|,
literal|"git notes prune"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|note_template
specifier|static
specifier|const
name|char
name|note_template
index|[]
init|=
literal|"\n"
literal|"#\n"
literal|"# Write/edit the notes for the following object:\n"
literal|"#\n"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|list_each_note
specifier|static
name|int
name|list_each_note
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|object_sha1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|note_sha1
parameter_list|,
name|char
modifier|*
name|note_path
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|note_sha1
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|object_sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|write_note_data
specifier|static
name|void
name|write_note_data
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|unsigned
name|long
name|size
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
name|buf
condition|)
block|{
if|if
condition|(
name|size
condition|)
name|write_or_die
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|write_commented_object
specifier|static
name|void
name|write_commented_object
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|object
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|show_args
index|[
literal|5
index|]
init|=
block|{
literal|"show"
block|,
literal|"--stat"
block|,
literal|"--no-notes"
block|,
name|sha1_to_hex
argument_list|(
name|object
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
name|struct
name|child_process
name|show
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|FILE
modifier|*
name|show_out
decl_stmt|;
comment|/* Invoke "git show --stat --no-notes $object" */
name|memset
argument_list|(
operator|&
name|show
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|show
argument_list|)
argument_list|)
expr_stmt|;
name|show
operator|.
name|argv
operator|=
name|show_args
expr_stmt|;
name|show
operator|.
name|no_stdin
operator|=
literal|1
expr_stmt|;
name|show
operator|.
name|out
operator|=
operator|-
literal|1
expr_stmt|;
name|show
operator|.
name|err
operator|=
literal|0
expr_stmt|;
name|show
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|show
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to start 'show' for object '%s'"
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
argument_list|)
argument_list|)
expr_stmt|;
comment|/* Open the output as FILE* so strbuf_getline() can be used. */
name|show_out
operator|=
name|xfdopen
argument_list|(
name|show
operator|.
name|out
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_out
operator|==
name|NULL
condition|)
name|die_errno
argument_list|(
literal|"can't fdopen 'show' output fd"
argument_list|)
expr_stmt|;
comment|/* Prepend "# " to each output line and write result to 'fd' */
while|while
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|buf
argument_list|,
name|show_out
argument_list|,
literal|'\n'
argument_list|)
operator|!=
name|EOF
condition|)
block|{
name|write_or_die
argument_list|(
name|fd
argument_list|,
literal|"# "
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|fd
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|buf
operator|.
name|len
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|fd
argument_list|,
literal|"\n"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|fclose
argument_list|(
name|show_out
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"failed to close pipe to 'show' for object '%s'"
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|finish_command
argument_list|(
operator|&
name|show
argument_list|)
condition|)
name|die
argument_list|(
literal|"failed to finish 'show' for object '%s'"
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|create_note
specifier|static
name|void
name|create_note
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|object
parameter_list|,
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|,
name|int
name|skip_editor
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|prev
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|)
block|{
name|char
modifier|*
name|path
init|=
name|NULL
decl_stmt|;
if|if
condition|(
operator|!
name|skip_editor
condition|)
block|{
name|int
name|fd
decl_stmt|;
comment|/* write the template message before editing: */
name|path
operator|=
name|git_pathdup
argument_list|(
literal|"NOTES_EDITMSG"
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|path
argument_list|,
name|O_CREAT
operator||
name|O_TRUNC
operator||
name|O_WRONLY
argument_list|,
literal|0600
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"could not create file '%s'"
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|prev
condition|)
name|write_note_data
argument_list|(
name|fd
argument_list|,
name|prev
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|fd
argument_list|,
name|note_template
argument_list|,
name|strlen
argument_list|(
name|note_template
argument_list|)
argument_list|)
expr_stmt|;
name|write_commented_object
argument_list|(
name|fd
argument_list|,
name|object
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
name|launch_editor
argument_list|(
name|path
argument_list|,
name|buf
argument_list|,
name|NULL
argument_list|)
condition|)
block|{
name|die
argument_list|(
literal|"Please supply the note contents using either -m"
expr|\
literal|" or -F option"
argument_list|)
expr_stmt|;
block|}
block|}
name|stripspace
argument_list|(
name|buf
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
operator|->
name|len
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Removing note for object %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
argument_list|)
argument_list|)
expr_stmt|;
name|hashclr
argument_list|(
name|result
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|write_sha1_file
argument_list|(
name|buf
operator|->
name|buf
argument_list|,
name|buf
operator|->
name|len
argument_list|,
name|blob_type
argument_list|,
name|result
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"unable to write note object"
argument_list|)
expr_stmt|;
if|if
condition|(
name|path
condition|)
name|error
argument_list|(
literal|"The note contents has been left in %s"
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|128
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
name|path
condition|)
block|{
name|unlink_or_warn
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|path
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_struct
DECL|struct|msg_arg
struct|struct
name|msg_arg
block|{
DECL|member|given
name|int
name|given
decl_stmt|;
DECL|member|buf
name|struct
name|strbuf
name|buf
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|parse_msg_arg
specifier|static
name|int
name|parse_msg_arg
parameter_list|(
specifier|const
name|struct
name|option
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|unset
parameter_list|)
block|{
name|struct
name|msg_arg
modifier|*
name|msg
init|=
name|opt
operator|->
name|value
decl_stmt|;
if|if
condition|(
operator|!
name|arg
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|msg
operator|->
name|buf
operator|.
name|len
condition|)
name|strbuf_addstr
argument_list|(
operator|&
operator|(
name|msg
operator|->
name|buf
operator|)
argument_list|,
literal|"\n\n"
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
operator|(
name|msg
operator|->
name|buf
operator|)
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|msg
operator|->
name|given
operator|=
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|commit_notes
name|int
name|commit_notes
parameter_list|(
name|struct
name|notes_tree
modifier|*
name|t
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|parent
decl_stmt|;
name|unsigned
name|char
name|tree_sha1
index|[
literal|20
index|]
decl_stmt|,
name|prev_commit
index|[
literal|20
index|]
decl_stmt|,
name|new_commit
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
operator|!
name|t
condition|)
name|t
operator|=
operator|&
name|default_notes_tree
expr_stmt|;
if|if
condition|(
operator|!
name|t
operator|->
name|initialized
operator|||
operator|!
name|t
operator|->
name|ref
operator|||
operator|!
operator|*
name|t
operator|->
name|ref
condition|)
name|die
argument_list|(
literal|"Cannot commit uninitialized/unreferenced notes tree"
argument_list|)
expr_stmt|;
comment|/* Prepare commit message and reflog message */
name|strbuf_addstr
argument_list|(
operator|&
name|buf
argument_list|,
literal|"notes: "
argument_list|)
expr_stmt|;
comment|/* commit message starts at index 7 */
name|strbuf_addstr
argument_list|(
operator|&
name|buf
argument_list|,
name|msg
argument_list|)
expr_stmt|;
if|if
condition|(
name|buf
operator|.
name|buf
index|[
name|buf
operator|.
name|len
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
name|strbuf_addch
argument_list|(
operator|&
name|buf
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
comment|/* Make sure msg ends with newline */
comment|/* Convert notes tree to tree object */
if|if
condition|(
name|write_notes_tree
argument_list|(
name|t
argument_list|,
name|tree_sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Failed to write current notes tree to database"
argument_list|)
expr_stmt|;
comment|/* Create new commit for the tree object */
if|if
condition|(
operator|!
name|read_ref
argument_list|(
name|t
operator|->
name|ref
argument_list|,
name|prev_commit
argument_list|)
condition|)
block|{
comment|/* retrieve parent commit */
name|parent
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|parent
argument_list|)
argument_list|)
expr_stmt|;
name|parent
operator|->
name|item
operator|=
name|lookup_commit
argument_list|(
name|prev_commit
argument_list|)
expr_stmt|;
name|parent
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
block|}
else|else
block|{
name|hashclr
argument_list|(
name|prev_commit
argument_list|)
expr_stmt|;
name|parent
operator|=
name|NULL
expr_stmt|;
block|}
if|if
condition|(
name|commit_tree
argument_list|(
name|buf
operator|.
name|buf
operator|+
literal|7
argument_list|,
name|tree_sha1
argument_list|,
name|parent
argument_list|,
name|new_commit
argument_list|,
name|NULL
argument_list|)
condition|)
name|die
argument_list|(
literal|"Failed to commit notes tree to database"
argument_list|)
expr_stmt|;
comment|/* Update notes ref with new commit */
name|update_ref
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
name|t
operator|->
name|ref
argument_list|,
name|new_commit
argument_list|,
name|prev_commit
argument_list|,
literal|0
argument_list|,
name|DIE_ON_ERR
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_notes
name|int
name|cmd_notes
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
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|notes_tree
modifier|*
name|t
decl_stmt|;
name|unsigned
name|char
name|object
index|[
literal|20
index|]
decl_stmt|,
name|new_note
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|unsigned
name|char
modifier|*
name|note
decl_stmt|;
specifier|const
name|char
modifier|*
name|object_ref
decl_stmt|;
name|char
name|logmsg
index|[
literal|100
index|]
decl_stmt|;
name|int
name|list
init|=
literal|0
decl_stmt|,
name|add
init|=
literal|0
decl_stmt|,
name|edit
init|=
literal|0
decl_stmt|,
name|show
init|=
literal|0
decl_stmt|,
name|remove
init|=
literal|0
decl_stmt|,
name|prune
init|=
literal|0
decl_stmt|,
name|force
init|=
literal|0
decl_stmt|;
name|int
name|given_object
decl_stmt|;
specifier|const
name|char
modifier|*
name|msgfile
init|=
name|NULL
decl_stmt|;
name|struct
name|msg_arg
name|msg
init|=
block|{
literal|0
block|,
name|STRBUF_INIT
block|}
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_GROUP
argument_list|(
literal|"Notes edit options"
argument_list|)
block|,
name|OPT_CALLBACK
argument_list|(
literal|'m'
argument_list|,
literal|"message"
argument_list|,
operator|&
name|msg
argument_list|,
literal|"msg"
argument_list|,
literal|"note contents as a string"
argument_list|,
name|parse_msg_arg
argument_list|)
block|,
name|OPT_FILENAME
argument_list|(
literal|'F'
argument_list|,
literal|"file"
argument_list|,
operator|&
name|msgfile
argument_list|,
literal|"note contents in a file"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'f'
argument_list|,
literal|"force"
argument_list|,
operator|&
name|force
argument_list|,
literal|"replace existing notes"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|options
argument_list|,
name|git_notes_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
literal|"list"
argument_list|)
condition|)
name|list
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argc
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
literal|"add"
argument_list|)
condition|)
name|add
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argc
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
literal|"edit"
argument_list|)
condition|)
name|edit
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argc
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
literal|"show"
argument_list|)
condition|)
name|show
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argc
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
literal|"remove"
argument_list|)
condition|)
name|remove
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|argc
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
literal|"prune"
argument_list|)
condition|)
name|prune
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|argc
condition|)
name|list
operator|=
literal|1
expr_stmt|;
comment|/* Default to 'list' if no other subcommand given */
if|if
condition|(
name|list
operator|+
name|add
operator|+
name|edit
operator|+
name|show
operator|+
name|remove
operator|+
name|prune
operator|!=
literal|1
condition|)
name|usage_with_options
argument_list|(
name|git_notes_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|msg
operator|.
name|given
operator|||
name|msgfile
operator|)
operator|&&
operator|!
operator|(
name|add
operator|||
name|edit
operator|)
condition|)
block|{
name|error
argument_list|(
literal|"cannot use -m/-F options with %s subcommand."
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|git_notes_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|msg
operator|.
name|given
operator|&&
name|msgfile
condition|)
block|{
name|error
argument_list|(
literal|"mixing -m and -F options is not allowed."
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|git_notes_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|force
operator|&&
operator|!
name|add
condition|)
block|{
name|error
argument_list|(
literal|"cannot use -f option with %s subcommand."
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|git_notes_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
name|given_object
operator|=
name|argc
operator|==
literal|2
expr_stmt|;
name|object_ref
operator|=
name|given_object
condition|?
name|argv
index|[
literal|1
index|]
else|:
literal|"HEAD"
expr_stmt|;
if|if
condition|(
name|argc
operator|>
literal|2
operator|||
operator|(
name|prune
operator|&&
name|argc
operator|>
literal|1
operator|)
condition|)
block|{
name|error
argument_list|(
literal|"too many parameters"
argument_list|)
expr_stmt|;
name|usage_with_options
argument_list|(
name|git_notes_usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|get_sha1
argument_list|(
name|object_ref
argument_list|,
name|object
argument_list|)
condition|)
name|die
argument_list|(
literal|"Failed to resolve '%s' as a valid ref."
argument_list|,
name|object_ref
argument_list|)
expr_stmt|;
name|init_notes
argument_list|(
name|NULL
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|t
operator|=
operator|&
name|default_notes_tree
expr_stmt|;
if|if
condition|(
name|prefixcmp
argument_list|(
name|t
operator|->
name|ref
argument_list|,
literal|"refs/notes/"
argument_list|)
condition|)
name|die
argument_list|(
literal|"Refusing to %s notes in %s (outside of refs/notes/)"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|,
name|t
operator|->
name|ref
argument_list|)
expr_stmt|;
name|note
operator|=
name|get_note
argument_list|(
name|t
argument_list|,
name|object
argument_list|)
expr_stmt|;
comment|/* list command */
if|if
condition|(
name|list
condition|)
block|{
if|if
condition|(
name|given_object
condition|)
block|{
if|if
condition|(
name|note
condition|)
block|{
name|puts
argument_list|(
name|sha1_to_hex
argument_list|(
name|note
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
block|}
else|else
return|return
name|for_each_note
argument_list|(
name|t
argument_list|,
literal|0
argument_list|,
name|list_each_note
argument_list|,
name|NULL
argument_list|)
return|;
block|}
comment|/* show command */
if|if
condition|(
operator|(
name|list
operator|||
name|show
operator|)
operator|&&
operator|!
name|note
condition|)
block|{
name|error
argument_list|(
literal|"No note found for object %s."
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
elseif|else
if|if
condition|(
name|show
condition|)
block|{
specifier|const
name|char
modifier|*
name|show_args
index|[
literal|3
index|]
init|=
block|{
literal|"show"
block|,
name|sha1_to_hex
argument_list|(
name|note
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
return|return
name|execv_git_cmd
argument_list|(
name|show_args
argument_list|)
return|;
block|}
comment|/* add/edit/remove/prune command */
if|if
condition|(
name|add
operator|&&
name|note
condition|)
block|{
if|if
condition|(
name|force
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Overwriting existing notes for object %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
argument_list|)
argument_list|)
expr_stmt|;
else|else
block|{
name|error
argument_list|(
literal|"Cannot add notes. Found existing notes for object %s. "
literal|"Use '-f' to overwrite existing notes"
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
block|}
if|if
condition|(
name|remove
condition|)
name|strbuf_reset
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|msg
operator|.
name|given
condition|)
name|strbuf_addbuf
argument_list|(
operator|&
name|buf
argument_list|,
operator|&
operator|(
name|msg
operator|.
name|buf
operator|)
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|msgfile
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|msgfile
argument_list|,
literal|"-"
argument_list|)
condition|)
block|{
if|if
condition|(
name|strbuf_read
argument_list|(
operator|&
name|buf
argument_list|,
literal|0
argument_list|,
literal|1024
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"cannot read '%s'"
argument_list|,
name|msgfile
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|strbuf_read_file
argument_list|(
operator|&
name|buf
argument_list|,
name|msgfile
argument_list|,
literal|1024
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"could not open or read '%s'"
argument_list|,
name|msgfile
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|prune
condition|)
block|{
name|hashclr
argument_list|(
name|new_note
argument_list|)
expr_stmt|;
name|prune_notes
argument_list|(
name|t
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|create_note
argument_list|(
name|object
argument_list|,
operator|&
name|buf
argument_list|,
name|msg
operator|.
name|given
operator|||
name|msgfile
operator|||
name|remove
argument_list|,
name|note
argument_list|,
name|new_note
argument_list|)
expr_stmt|;
if|if
condition|(
name|is_null_sha1
argument_list|(
name|new_note
argument_list|)
condition|)
name|remove_note
argument_list|(
name|t
argument_list|,
name|object
argument_list|)
expr_stmt|;
else|else
name|add_note
argument_list|(
name|t
argument_list|,
name|object
argument_list|,
name|new_note
argument_list|,
name|combine_notes_overwrite
argument_list|)
expr_stmt|;
block|}
name|snprintf
argument_list|(
name|logmsg
argument_list|,
sizeof|sizeof
argument_list|(
name|logmsg
argument_list|)
argument_list|,
literal|"Note %s by 'git notes %s'"
argument_list|,
name|is_null_sha1
argument_list|(
name|new_note
argument_list|)
condition|?
literal|"removed"
else|:
literal|"added"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|commit_notes
argument_list|(
name|t
argument_list|,
name|logmsg
argument_list|)
expr_stmt|;
name|free_notes
argument_list|(
name|t
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
