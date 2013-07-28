
collections = {
    %'acm_mirum_tempos.mf'
    %'ballroom_tempos.mf'
    'genres_tempos.mf'
    %'ismir2004_songs_tempos.mf'
    %'hains_tempos.mf'
    };
md = getenv('MARSYAS_DATADIR');

for coll_index = 1:length(collections)
	coll_name = collections(coll_index);
	coll = char(strcat(md, coll_name));
	%coll = "foo.mf";
	fid = fopen(coll,'r');
	j = 1;
	wavs = {};
	bpms = zeros();
	while ~feof(fid)
		wavs(j) = fscanf(fid, "%s\t", 1);
		bpms(j) = fscanf(fid, "%f");
		j += 1;
	end
	fclose(fid);

	outfile = coll(1:end-3);
	outfile = strrep(outfile, md, "");
	outfile = strcat(outfile, '-mar-matlab.mf');
	fout = fopen(outfile, 'w');

	for i = 1:length(wavs)
		wav = char(wavs(i));
		wav = strrep(wav, 'MARSYAS_DATADIR', md);
		%bpm_ground = bpms(i);

		bpm = tempo_stem_file(wav);
		fprintf(fout, '%s\t%f\n', wav, bpm);
		printf("Completed file %i / %i\n", i, length(wavs));
		fflush(fid);
	end
	fclose(fout);
end

