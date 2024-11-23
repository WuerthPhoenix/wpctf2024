for i in {1..50}   # you can also use {0..9}
do
  curl --output "pic_$i.jpg" https://thispersondoesnotexist.com/ 
done
