for i in {1..1000}
do
  echo "Iteration $i"
  ./imprise | python3 h5_script.py
  sleep 5
done