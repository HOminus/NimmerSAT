using Plots;
gr()

function cactus(path)
    lines = readlines(path)
    data = Vector{Float32}()
    for l in lines
        t = tryparse(Float32, l)
        if t !== nothing
            push!(data, t)
        end
    end
    for i in 2:length(data)
        data[i] = data[i] + data[i - 1]
    end
    return data
end

#=
data = cactus("measurements/formulas_sat_debug.data")
p = plot(data, label="Debug")

data = cactus("measurements/formulas_sat_release.data")
plot!(data, label="Release")
=#

data = cactus("measurements/2007SATindustrial_picosat.data")
p = plot(data, label="Picosat")

data = cactus("measurements/2007SATindustrial_minisat.data")
plot!(data, label="Minisat")

display(p)
