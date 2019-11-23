for f in *.json;
do
  node ../../../awtk-mvvm/tools/gen_vm.js $f
done

