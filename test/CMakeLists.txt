# 1. Check that we can run the most trivial example
add_test(NAME minimal-run COMMAND muonargon -m "${CMAKE_CURRENT_LIST_DIR}/test0.mac")

# 2. Check trajectory storage runs
add_test(NAME trajectory-storage COMMAND muonargon -m "${CMAKE_CURRENT_LIST_DIR}/test-store-trajectory.mac")
