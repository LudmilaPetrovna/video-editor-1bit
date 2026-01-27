$width=0;
$height=0;

$target_width=128;
$target_height=64;
$target_aspect=$target_width/$target_height;

$info=`ffprobe -unit -show_streams $ARGV[0] 2>&1`;
if($info=~/^width=(\d+)/m){$width=$1;}
if($info=~/^height=(\d+)/m){$height=$1;}

$aspect=$width/$height;
print "We have video ${width}x${height}, aspect: $aspect\n";

$try_width=$target_width;
$try_height=$target_width/$aspect;
$try1=[$try_width,$try_height];
print "Try1: 128x72   $try_width x $try_height\n";

$try_height=$target_height;
$try_width=$target_height*$aspect;
$try2=[$try_width,$try_height];
print "Try2: 114x64   $try_width x $try_height\n";


$new_target=$try1->[0]>$try2->[0]?$try1:$try2;
$new_target=[map{int($_)}@{$new_target}];

$offset_x=int(-($target_width-$new_target->[0])/2);
$offset_y=int(-($target_height-$new_target->[1])/2);

print "new resolution: $new_target->[0] x $new_target->[1] +$offset_x +$offset_y\n";



