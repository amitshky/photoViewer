#! /bin/sh

SESSION="photoV"

# check if the session exists
tmux has-session -t $SESSION 2>/dev/null

# if the session doesn't exists
if [ $? != 0 ]; then
    # create a session with a window called "editor"
    tmux new-session -d -s $SESSION -n "editor"

    tmux send-keys -t $SESSION:editor "nvim -S session.vim -c \"bd ~/dev/photoViewer\"" C-m

    # create another window called "git"
    tmux new-window -t $SESSION -n "lazygit"
    tmux send-keys -t $SESSION:lazygit "lazygit" C-m

    tmux new-window -t $SESSION -n "term"

    tmux select-window -t $SESSION:editor
fi

# attach the session if it exists
tmux attach-session -t $SESSION

