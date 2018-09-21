require "RMagick"
require 'ftools'

path = "./test/inlay/."

def makeThumbnails fname
   img = Magick::Image::read(fname).first

   scale = 0.
   if img.columns > img.rows
      scale = img.columns/250.
   else
      scale = img.rows/250.
   end

   puts img.columns.to_s
   puts img.rows.to_s
   puts scale.to_s

   new_fname = File.split(fname)

   File.makedirs(File.join(new_fname[0], ".thumbnails"))
puts new_fname[0].to_s
puts new_fname[1].to_s
   command = "convert -antialias -strip -thumbnail 300 '"+fname+"' '"+File.join(new_fname[0], ".thumbnails", new_fname[1])+"'"
puts command
   system command
puts "Done"
end

def buildImageList path
   image_list = []

   Dir.foreach(path) do |image_name|
      image_list.push(File.join(path, image_name)) if /^.*?\.(jpg|jpeg|tiff|tif|gif|png)$/.match(image_name)
   end

   image_list
end

list = buildImageList path

puts list.to_s

