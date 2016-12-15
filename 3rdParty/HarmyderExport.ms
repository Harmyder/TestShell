proposed_fn = maxFilePath+(getFilenameFile maxFileName) + ".hrm"
filename = getSaveFileName \
	caption:"Save file" \
	filename: proposed_fn\
	types:"Harmyder files (*.hrm)|*.hrm|All (*.*)|*.*|"
	
if (filename != undefined) then (
	outfile = createfile filename

	for obj in Geometry do (
		if (classOf obj == Box) 
		then (
			format "name=% type=Box length=% width=% height=%\n" obj.name obj.height obj.width obj.length to:outfile
		)

		if (classOf obj == Sphere) 
		then (
			format "name=% type=Sphere radius=%\n" obj.name obj.radius to:outfile
		)

		if (classOf obj == Capsule) 
		then (
			format "name=% type=Capsule radius=% height=%\n" obj.name obj.radius obj.height to:outfile
		)
	)

	close outfile
)