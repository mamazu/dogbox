#!/usr/bin/env bash
SOURCE=`pwd`/../dogbox
cmake --build . || exit 1
fusermount -u ./fuse_mount
rmdir fuse_mount
mkdir -p fuse_mount || exit 1
./fuse_frontend/fuse_frontend $SOURCE -f ./fuse_mount &
sleep 1s
FUSE_FRONTEND_PID=$!
diff -r $SOURCE ./fuse_mount
DIFF_RESULT=$?
fusermount -u ./fuse_mount || exit 1
wait $FUSE_FRONTEND_PID
exit $DIFF_RESULT
